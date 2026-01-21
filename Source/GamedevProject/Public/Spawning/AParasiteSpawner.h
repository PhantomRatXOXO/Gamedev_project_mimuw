#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AParasiteSpawner.generated.h"

class AEnemyBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FParasiteLevelClearedSignature);

UCLASS(Blueprintable)
class GAMEDEVPROJECT_API AParasiteSpawner : public AActor
{
	GENERATED_BODY()

public:
	AParasiteSpawner();

	virtual void BeginPlay() override;

	// Set this to your BP_Parasite (which should inherit from AParasiteEnemy)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	TSubclassOf<AEnemyBase> ParasiteClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning", meta=(ClampMin="0"))
	int32 MaxTotalSpawned = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning", meta=(ClampMin="0.0"))
	float MinDistanceFromPlayer = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning", meta=(ClampMin="0.0"))
	float MaxDistanceFromPlayer = 5000.f;

	// Spawn pacing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning|Rate", meta=(ClampMin="0.01"))
	float InitialInterval = 6.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning|Rate", meta=(ClampMin="0.01"))
	float MinInterval = 1.f;

	// Linear ramp: every minute interval decreases by this amount until MinInterval
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning|Rate", meta=(ClampMin="0.0"))
	float IntervalDecreasePerMinute = 0.8f;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FParasiteLevelClearedSignature OnLevelCleared;

	UFUNCTION(BlueprintCallable, Category="Spawning")
	bool IsCleared() const;

protected:
	int32 TotalSpawned = 0;
	int32 AliveCount = 0;
	float StartTime = 0.f;

	FTimerHandle SpawnTimer;

	void ScheduleNextSpawn();
	void DoSpawn();

	UFUNCTION()
	void HandleSpawnedDied(AActor* DeadActor);

	float GetCurrentInterval() const;
};
