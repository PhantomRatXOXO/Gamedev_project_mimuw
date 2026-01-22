#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "InputActionValue.h"
#include "MainCharacter.generated.h"

// Forward declarations to keep compile times fast
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UUserWidget;
class UHealthBarWidget;
class UWidgetComponent;

UCLASS(Blueprintable)
class GAMEDEVPROJECT_API AMainCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

protected:
	virtual void BeginPlay() override;

	// Override to handle player-specific death (game over, etc.)
	virtual void Die_Implementation() override;

	// Override to update player health bar UI
	virtual void HandleHealthChanged(float CurrentHealth, float InMaxHealth) override;

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

	/** ATTACK INPUT **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;

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

	/** UI **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> PlayerHealthWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> PlayerHealthWidgetComponent = nullptr;

	/** ATTACK MONTAGE **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* AttackMontage = nullptr;

public:
	// Called from AnimNotify in the attack montage to apply damage
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformAttackCheck();

private:
	void Attack();

	UPROPERTY()
	TObjectPtr<UHealthBarWidget> PlayerHealthWidget = nullptr;

	// This timer handles the cooldown so players can't spam spacebar to fly.
	FTimerHandle DashTimer;
	bool bCanDash = true; // State tracker

	// The function we will bind to Spacebar
	void Dash();

	// The function called when the timer finishes
	void ResetDash();

	/** COMBAT CONFIG **/
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackDamage = 15.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 250.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackHitRadius = 140.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackCooldown = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TEnumAsByte<ECollisionChannel> AttackHitChannel = ECC_Pawn;

	bool bCanAttack = true;
	FTimerHandle AttackCooldownHandle;
};
