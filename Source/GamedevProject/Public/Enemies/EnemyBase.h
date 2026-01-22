#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "EnemyBase.generated.h"

class UAnimMontage;
class UUserWidget;
class UWidgetComponent;
class UHealthBarWidget;

// Enemy-specific death delegate (passes enemy pointer for spawner tracking)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyDiedSignature, AEnemyBase*, Enemy);

UCLASS(Abstract)
class GAMEDEVPROJECT_API AEnemyBase : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

	// Enemy-specific death event (used by spawner)
	UPROPERTY(BlueprintAssignable, Category="Enemy|Events")
	FEnemyDiedSignature OnEnemyDied;

	UFUNCTION(BlueprintCallable, Category="Enemy|Combat")
	void TryAttack(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category="Enemy|Combat")
	bool IsAttacking() const { return bIsAttacking; }

	UFUNCTION(BlueprintCallable, Category="Enemy|Combat")
	float GetAttackRange() const { return AttackRange; }

protected:
	virtual void BeginPlay() override;

	// Override base class Die to add enemy-specific behavior
	virtual void Die_Implementation() override;

	// Override to update enemy health bar UI
	virtual void HandleHealthChanged(float CurrentHealth, float InMaxHealth) override;

	void DealMeleeDamage();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Combat")
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Combat")
	float AttackRange = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Combat")
	float AttackCooldown = 1.2f;

	// Delay between starting montage and applying damage (lets you match hit timing without AnimNotifies).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Combat")
	float AttackDamageDelay = 0.25f;

	// Radius of the melee hit check.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Combat")
	float AttackHitRadius = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Combat")
	UAnimMontage* AttackMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Combat")
	TEnumAsByte<ECollisionChannel> AttackHitChannel = ECC_Pawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|UI")
	TObjectPtr<UWidgetComponent> HealthWidgetComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|UI")
	TSubclassOf<UUserWidget> EnemyHealthWidgetClass;

private:
	void SetupHealthWidget();

	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget = nullptr;

	bool bCanAttack = true;
	bool bIsAttacking = false;

	FTimerHandle AttackCooldownHandle;
	FTimerHandle AttackDamageHandle;
};
