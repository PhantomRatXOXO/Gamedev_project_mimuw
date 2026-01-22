#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedSignature, ABaseCharacter*, Character);

UCLASS(Abstract)
class GAMEDEVPROJECT_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	// UE damage pipeline entry point
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsAlive() const { return Health > 0.f; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetHealthPercent() const { return MaxHealth > 0.f ? (Health / MaxHealth) : 0.f; }

	// Broadcasts when health changes (for UI, etc.)
	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FHealthChangedSignature OnHealthChanged;

	// Broadcasts when character dies
	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FCharacterDiedSignature OnCharacterDied;

protected:
	virtual void BeginPlay() override;

	// Called when health reaches zero - override for custom death behavior
	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void Die();
	virtual void Die_Implementation();

	// Override this to handle health UI updates in subclasses
	UFUNCTION()
	virtual void HandleHealthChanged(float CurrentHealth, float InMaxHealth);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float Health = 100.f;

	// Optional death animation montage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* DeathMontage = nullptr;
};
