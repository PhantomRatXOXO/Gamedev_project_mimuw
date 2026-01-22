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
class UUserWidget;
class UHealthBarWidget;
class UWidgetComponent;

UCLASS(Blueprintable)
class GAMEDEVPROJECT_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerHealthChangedSignature, float, CurrentHealth, float, MaxHealth);


	// Sets default values for this character's properties
	AMainCharacter();

	// UE damage pipeline entry point.
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	UFUNCTION(BlueprintCallable, Category="Combat")
	bool IsAlive() const { return Health > 0.f; }

	UFUNCTION(BlueprintCallable, Category="Combat")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable, Category="Combat")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category="Combat")
	float GetHealthPercent() const { return MaxHealth > 0.f ? (Health / MaxHealth) : 0.f; }

	UPROPERTY(BlueprintAssignable, Category="Combat|Events")
	FPlayerHealthChangedSignature OnHealthChanged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleHealthChanged(float CurrentHealth, float MaxHealth);

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

	/** COMBAT **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float Health = 100.f;

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
