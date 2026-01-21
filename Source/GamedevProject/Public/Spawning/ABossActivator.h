#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABossActivator.generated.h"

class UBoxComponent;
class AParasiteSpawner;
class AEnemyBase;

UCLASS(Blueprintable)
class GAMEDEVPROJECT_API ABossActivator : public AActor
{
	GENERATED_BODY()

public:
	ABossActivator();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> Trigger;

	// Set to BP_Boss (inherits from ABossEnemy)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss")
	TSubclassOf<AEnemyBase> BossClass;

	// Assign in level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss")
	TObjectPtr<AParasiteSpawner> SpawnerRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss", meta=(ClampMin="0.0"))
	float MinBossDistanceFromPlayer = 2500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss", meta=(ClampMin="0.0"))
	float MaxBossDistanceFromPlayer = 8000.f;

protected:
	bool bBossFightEnabled = false;
	bool bBossSpawned = false;

	UFUNCTION()
	void HandleLevelCleared();

	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void SpawnBoss();
};
