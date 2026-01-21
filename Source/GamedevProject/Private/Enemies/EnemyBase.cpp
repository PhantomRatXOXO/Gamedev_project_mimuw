#include "Enemies/EnemyBase.h"

#include "AI/EnemyAIController.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "CollisionShape.h"


AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

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
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return 0.f;
	}

	const float Applied = FMath::Max(0.f, DamageAmount);
	Health -= Applied;

	if (Health <= 0.f)
	{
		Die();
	}
	return Applied;
}

void AEnemyBase::Die()
{
	// Stop movement and collisions, then destroy.
	if (AController* C = GetController())
	{
		C->StopMovement();
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnEnemyDied.Broadcast(this);

	Destroy();
}

void AEnemyBase::TryAttack(AActor* TargetActor)
{
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
