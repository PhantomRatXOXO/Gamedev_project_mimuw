#include "Enemies/EnemyBase.h"

#include "AI/EnemyAIController.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "CollisionShape.h"
#include "Components/WidgetComponent.h"
#include "UI/HealthBarWidget.h"


AEnemyBase::AEnemyBase()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Make sure AI possesses both placed and spawned enemies.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEnemyAIController::StaticClass();

	// Typical defaults for AI characters.
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->bOrientRotationToMovement = true;
		Move->RotationRate = FRotator(0.f, 540.f, 0.f);
		Move->MaxWalkSpeed = 250.f;
	}
	bUseControllerRotationYaw = false;

	HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidgetComponent->SetupAttachment(RootComponent);
	HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthWidgetComponent->SetDrawSize(FVector2D(120.f, 12.f));
	HealthWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	HealthWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthWidgetComponent->SetGenerateOverlapEvents(false);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	SetupHealthWidget();
	// Bind health changes to UI update
	OnHealthChanged.AddDynamic(this, &AEnemyBase::HandleHealthChanged);
	OnHealthChanged.Broadcast(Health, MaxHealth);
}

void AEnemyBase::HandleHealthChanged(float CurrentHealth, float InMaxHealth)
{
	if (HealthWidgetComponent)
	{
		if (UHealthBarWidget* Widget = Cast<UHealthBarWidget>(HealthWidgetComponent->GetUserWidgetObject()))
		{
			const float Percent = InMaxHealth > 0.f ? (CurrentHealth / InMaxHealth) : 0.f;
			Widget->SetHealthPercent(Percent);
		}
	}
}

void AEnemyBase::Die_Implementation()
{
	// Call base class to handle common death logic (stop movement, disable collision, play death montage)
	Super::Die_Implementation();

	// Enemy-specific: notify spawner
	OnEnemyDied.Broadcast(this);

	// Destroy after a short delay if death montage is playing, otherwise immediately
	if (DeathMontage)
	{
		const float MontageLength = DeathMontage->GetPlayLength();
		FTimerHandle DestroyHandle;
		GetWorldTimerManager().SetTimer(
			DestroyHandle,
			[this]() { Destroy(); },
			MontageLength,
			false
		);
	}
	else
	{
		Destroy();
	}
}

void AEnemyBase::TryAttack(AActor* TargetActor)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy TryAttack: %s Target=%s CanAttack=%d Alive=%d"),
		*GetName(), TargetActor ? *TargetActor->GetName() : TEXT("None"), bCanAttack ? 1 : 0, IsAlive() ? 1 : 0);
	if (!IsAlive() || !bCanAttack || !IsValid(TargetActor))
	{
		return;
	}

	const float Dist = FVector::Dist2D(GetActorLocation(), TargetActor->GetActorLocation());
	if (Dist > AttackRange)
	{
		return;
	}

	bCanAttack = false;
	bIsAttacking = true;
	CurrentTarget = TargetActor;

	if (AController* C = GetController())
	{
		C->StopMovement();
	}
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
	}

	// Face the target so melee checks are consistent with the animation.
	const FVector ToTarget = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	if (!ToTarget.IsNearlyZero())
	{
		const FRotator FaceRot = ToTarget.Rotation();
		SetActorRotation(FRotator(0.f, FaceRot.Yaw, 0.f));
	}

	// Play montage if provided (purely cosmetic; damage timing handled via AttackDamageDelay).
	UE_LOG(LogTemp, Warning, TEXT("Enemy AttackMontage: %s Montage=%s"),
		*GetName(), AttackMontage ? *AttackMontage->GetName() : TEXT("None"));
	if (AttackMontage)
	{
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
			{
				Anim->Montage_Play(AttackMontage);
			}
		}
	}

	// Apply damage after a small delay (hit timing).
	GetWorldTimerManager().ClearTimer(AttackDamageHandle);
	GetWorldTimerManager().SetTimer(
		AttackDamageHandle,
		this,
		&AEnemyBase::DealMeleeDamage,
		AttackDamageDelay,
		false
	);

	// Cooldown gate.
	GetWorldTimerManager().ClearTimer(AttackCooldownHandle);
	GetWorldTimerManager().SetTimer(
		AttackCooldownHandle,
		[this]()
		{
			bCanAttack = true;
			bIsAttacking = false;
		},
		AttackCooldown,
		false
	);
}

void AEnemyBase::DealMeleeDamage()
{
	if (!IsAlive() || !IsValid(CurrentTarget))
	{
		return;
	}

	// Sweep a small sphere in front of the enemy to emulate a punch.
	const FVector Start = GetActorLocation();
	const FVector End = Start + GetActorForwardVector() * AttackRange;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(EnemyMelee), false, this);
	FCollisionShape Shape = FCollisionShape::MakeSphere(AttackHitRadius);

	TArray<FHitResult> Hits;
	const bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		AttackHitChannel,
		Shape,
		Params
	);

	if (!bHit)
	{
		return;
	}

	for (const FHitResult& Hit : Hits)
	{
		if (Hit.GetActor() == CurrentTarget)
		{
			UGameplayStatics::ApplyDamage(
				CurrentTarget,
				Damage,
				GetController(),
				this,
				UDamageType::StaticClass()
			);
			break;
		}
	}
}

void AEnemyBase::SetupHealthWidget()
{
	if (!HealthWidgetComponent)
	{
		return;
	}

	if (EnemyHealthWidgetClass)
	{
		HealthWidgetComponent->SetWidgetClass(EnemyHealthWidgetClass);
		HealthWidgetComponent->InitWidget();
	}

	OnHealthChanged.AddDynamic(this, &AEnemyBase::HandleHealthChanged);
	HandleHealthChanged(Health, MaxHealth);
}
