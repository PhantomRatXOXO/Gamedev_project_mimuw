#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UnitBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitDiedSignature, AUnitBase*, Unit);

/**
 * Base class for all combat units (player and enemies).
 * Provides shared health, damage, and death functionality.
 */
UCLASS(Abstract)
class GAMEDEVPROJECT_API AUnitBase : public ACharacter
{
	GENERATED_BODY()

public:
	AUnitBase();

	// UE damage pipeline: supports UGameplayStatics::ApplyDamage
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	UPROPERTY(BlueprintAssignable, Category="Unit|Events")
	FUnitDiedSignature OnUnitDied;

	UFUNCTION(BlueprintCallable, Category="Unit|Combat")
	bool IsAlive() const { return Health > 0.f; }

	UFUNCTION(BlueprintCallable, Category="Unit|Combat")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable, Category="Unit|Combat")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category="Unit|Combat")
	float GetHealthPercent() const { return MaxHealth > 0.f ? Health / MaxHealth : 0.f; }

protected:
	virtual void BeginPlay() override;

	// Called when health reaches zero. Override for custom death behavior.
	virtual void Die();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Unit|Combat")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Unit|Combat")
	float Health = 100.f;
};
