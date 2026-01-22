#include "Characters/MainCharacter.h"
#include "Characters/BaseCharacter.h"
#include "Enemies/EnemyBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraFunctionLibrary.h" // Required for spawning particles
#include "TimerManager.h"           // Required for the cooldown timer
#include "Components/CapsuleComponent.h"
#include "UI/HealthBarWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CollisionShape.h"
#include "GameFramework/DamageType.h"
#include "Engine/EngineTypes.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f); // ...at this rotation rate

	// Note: For top-down games, stopping quickly usually feels better
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->MaxWalkSpeed = 800.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	// --- HADES VIEW SETTINGS ---
	CameraBoom->TargetArmLength = 1500.0f; // The distance from the character
	CameraBoom->SetRelativeRotation(FRotator(-50.f, 225.f, 0.f)); // Look down at 50 degrees, rotated 180 degrees from previous view
	CameraBoom->bUsePawnControlRotation = false; // Do not rotate camera with mouse
	CameraBoom->bDoCollisionTest = false; // Don't pull camera in when hitting walls (better for top-down)
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PlayerHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerHealthWidget"));
	PlayerHealthWidgetComponent->SetupAttachment(RootComponent);
	PlayerHealthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	PlayerHealthWidgetComponent->SetDrawSize(FVector2D(160.f, 14.f));
	PlayerHealthWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	PlayerHealthWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayerHealthWidgetComponent->SetGenerateOverlapEvents(false);
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Setup player health widget
	if (PlayerHealthWidgetComponent && PlayerHealthWidgetClass)
	{
		PlayerHealthWidgetComponent->SetWidgetClass(PlayerHealthWidgetClass);
		PlayerHealthWidgetComponent->InitWidget();
		PlayerHealthWidget = Cast<UHealthBarWidget>(PlayerHealthWidgetComponent->GetUserWidgetObject());
		if (PlayerHealthWidget)
		{
			PlayerHealthWidget->SetHealthPercent(GetHealthPercent());
		}
	}

	// Bind health changes to UI update
	OnHealthChanged.AddDynamic(this, &AMainCharacter::HandleHealthChanged);
	OnHealthChanged.Broadcast(Health, MaxHealth);

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void AMainCharacter::Die_Implementation()
{
	// Call base class to handle common death logic
	Super::Die_Implementation();

	// Show game over screen after delay (for death animation)
	if (GameOverWidgetClass)
	{
		FTimerHandle GameOverHandle;
		GetWorldTimerManager().SetTimer(
			GameOverHandle,
			[this]()
			{
				if (APlayerController* PC = Cast<APlayerController>(GetController()))
				{
					if (UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(PC, GameOverWidgetClass))
					{
						GameOverWidget->AddToViewport(100);
					}
				}
			},
			GameOverDelay,
			false
		);
	}
}

void AMainCharacter::HandleHealthChanged(float CurrentHealth, float InMaxHealth)
{
	if (!PlayerHealthWidget)
	{
		return;
	}

	const float Percent = InMaxHealth > 0.f ? (CurrentHealth / InMaxHealth) : 0.f;
	PlayerHealthWidget->SetHealthPercent(Percent);
}


// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Moving
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		}

		// Inside SetupPlayerInputComponent...
		if (DashAction)
		{
			EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AMainCharacter::Dash);
		}

		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AMainCharacter::Attack);
		}
	}
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// FIX: Use the CAMERA's rotation, not the Controller's!
		// This ensures "Forward" is always relative to what the player sees.
		const FRotator Rotation = FollowCamera->GetComponentRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// Get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMainCharacter::Dash()
{
	// 1. Check Cooldown
	if (!bCanDash) return;

	// 2. Find Dash Direction
	// Use the player's last input vector (WASD). 
	FVector DashDirection = GetLastMovementInputVector();

	// Fallback: If player is standing still (Input is Zero), dash forward
	if (DashDirection.IsNearlyZero())
	{
		DashDirection = GetActorForwardVector();
	}

	// 3. Apply Physics (The "Launch")
	// true, true = Override current velocity entirely (crisp movement)
	LaunchCharacter(DashDirection * DashImpulse, true, true);
	OnDashStart();

	// 4. Play Particles
	if (DashVFX)
	{
		// Spawns the effect at the character's feet, facing the dash direction
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			DashVFX,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	// 5. Start Cooldown
	bCanDash = false;
	GetWorldTimerManager().SetTimer(DashTimer, this, &AMainCharacter::ResetDash, DashCooldownTime, false);
}

void AMainCharacter::Attack()
{
	if (!IsAlive() || !bCanAttack)
	{
		return;
	}

	bCanAttack = false;

	// Play the attack animation - damage will be applied via AnimNotify calling PerformAttackCheck()
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
	else
	{
		// Fallback: no montage assigned, apply damage immediately (old behavior)
		PerformAttackCheck();
	}

	GetWorldTimerManager().ClearTimer(AttackCooldownHandle);
	GetWorldTimerManager().SetTimer(
		AttackCooldownHandle,
		[this]()
		{
			bCanAttack = true;
		},
		AttackCooldown,
		false
	);
}

void AMainCharacter::PerformAttackCheck()
{
	// Forward sweep: starts at player, ends in front
	const FVector Start = GetActorLocation();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(PlayerMelee), false, this);
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

	UE_LOG(LogTemp, Warning, TEXT("Player Attack (Forward): Range=%.1f Radius=%.1f Hits=%d"),
		AttackRange, AttackHitRadius, Hits.Num());

	if (bHit)
	{
		// Track which enemies we've already damaged (in case of duplicate hits)
		TSet<AActor*> DamagedActors;

		for (const FHitResult& Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();

			// Skip if already damaged, not valid, or is self
			if (!HitActor || HitActor == this || DamagedActors.Contains(HitActor))
			{
				continue;
			}

			// Only damage enemies
			if (HitActor->IsA(AEnemyBase::StaticClass()))
			{
				UGameplayStatics::ApplyDamage(
					HitActor,
					AttackDamage,
					GetController(),
					this,
					UDamageType::StaticClass()
				);
				DamagedActors.Add(HitActor);
			}
		}
	}
}

void AMainCharacter::ResetDash()
{
	bCanDash = true;
}
