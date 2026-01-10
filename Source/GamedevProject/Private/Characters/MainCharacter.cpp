#include "Characters/MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraFunctionLibrary.h" // Required for spawning particles
#include "TimerManager.h"           // Required for the cooldown timer

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
	CameraBoom->SetRelativeRotation(FRotator(-50.f, 45.f, 0.f)); // Look down at 50 degrees, rotated 45 degrees to the right
	CameraBoom->bUsePawnControlRotation = false; // Do not rotate camera with mouse
	CameraBoom->bDoCollisionTest = false; // Don't pull camera in when hitting walls (better for top-down)
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

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

void AMainCharacter::ResetDash()
{
	bCanDash = true;
}