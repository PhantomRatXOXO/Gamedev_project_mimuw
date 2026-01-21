#pragma once

#include "CoreMinimal.h"
#include "Characters/ABaseCharacter.h"
#include "AEnemyBase.generated.h"

UCLASS()
class GAMEDEVPROJECT_API AEnemyBase : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

protected:
	virtual void BeginPlay() override;

	// Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(ClampMin="0.0"))
	float AttackRange = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(ClampMin="0.0"))
	float AttackCooldown = 1.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bCanAttack = true;

	FTimerHandle AttackCooldownTimer;

	UFUNCTION(BlueprintCallable, Category="Combat")
	void DealDamage(AActor* Target);

	// Optional: enemy-specific death behavior (leave base default if you want)
	virtual void Die() override;

public:
	UFUNCTION(BlueprintCallable, Category="Combat")
	bool TryAttack(AActor* Target);

	// Expose AttackRange if BT/AI needs it
	UFUNCTION(BlueprintCallable, Category="Combat")
	float GetAttackRange() const { return AttackRange; }
};
