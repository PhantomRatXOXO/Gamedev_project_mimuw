#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MainCharacter.generated.h"

// Forward declarations to keep compile times fast
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS(Blueprintable)
class GAMEDEVPROJECT_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** DASH INPUT **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DashAction;

	// Allow Blueprint to handle the visuals when dash starts/stops
	UFUNCTION(BlueprintImplementableEvent, Category = "Effects")
	void OnDashStart();

	/** DASH CONFIG **/
	UPROPERTY(EditAnywhere, Category = "Movement | Dash")
	float DashImpulse = 3000.0f; // Force of the dash. 3000 is usually a good snappy start.

	UPROPERTY(EditAnywhere, Category = "Movement | Dash")
	float DashCooldownTime = 1.0f; // How often can they dash?

	/** VFX **/
	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraSystem* DashVFX; // The particle effect asset

private:
	// This timer handles the cooldown so players can't spam spacebar to fly.
	FTimerHandle DashTimer;
	bool bCanDash = true; // State tracker

	// The function we will bind to Spacebar
	void Dash();

	// The function called when the timer finishes
	void ResetDash();
};