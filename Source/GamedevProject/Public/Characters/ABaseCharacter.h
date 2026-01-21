#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ABaseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBaseCharacterDiedSignature, AActor*, DeadActor);

UCLASS(Blueprintable)
class GAMEDEVPROJECT_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;

	// Shared Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", meta=(ClampMin="0.0"))
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats")
	float CurrentHealth = 100.f;

public:
	// Shared helpers
	UFUNCTION(BlueprintCallable, Category="Stats")
	bool IsDead() const { return CurrentHealth <= 0.f; }

	UFUNCTION(BlueprintCallable, Category="Stats")
	float GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category="Stats")
	float GetMaxHealth() const { return MaxHealth; }

	// Built-in damage hook (works for Player + Enemy)
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	// Spawners and other systems bind to this
	UPROPERTY(BlueprintAssignable, Category="Events")
	FBaseCharacterDiedSignature OnDied;

protected:
	// Blueprint hooks for visuals (optional)
	UFUNCTION(BlueprintImplementableEvent, Category="VFX")
	void BP_OnHit(float DamageAmount);

	UFUNCTION(BlueprintImplementableEvent, Category="VFX")
	void BP_OnDeath();

	// MUST be virtual because EnemyBase overrides it
	virtual void Die();
};
