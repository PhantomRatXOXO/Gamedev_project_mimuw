#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyDiedSignature, AEnemyBase*, Enemy);

UCLASS(Abstract)
class GAMEDEVPROJECT_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

	// UE damage pipeline: supports UGameplayStatics::ApplyDamage from enemy to player, and vice versa.
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	UPROPERTY(BlueprintAssignable, Category="Enemy|Events")
	FEnemyDiedSignature OnEnemyDied;

	UFUNCTION(BlueprintCallable, Category="Enemy|Combat")
	void TryAttack(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category="Enemy|Combat")
	bool IsAlive() const { return Health > 0.f; }

	UFUNCTION(BlueprintCallable, Category="Enemy|Combat")
	bool IsAttacking() const { return bIsAttacking; }

	UFUNCTION(BlueprintCallable, Category="Enemy|Combat")
	float GetAttackRange() const { return AttackRange; }

protected:
	virtual void BeginPlay() override;

	void Die();

	void DealMeleeDamage();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Combat")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|Combat")
	float Health = 100.f;

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

private:
	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget = nullptr;

	bool bCanAttack = true;
	bool bIsAttacking = false;

	FTimerHandle AttackCooldownHandle;
	FTimerHandle AttackDamageHandle;
};
