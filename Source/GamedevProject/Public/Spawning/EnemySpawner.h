#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AEnemyBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpawnerClearedSignature);

UCLASS()
class GAMEDEVPROJECT_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

	UPROPERTY(BlueprintAssignable, Category="Spawning|Logic")
	FSpawnerClearedSignature OnLevelCleared;

protected:
	virtual void BeginPlay() override;

private:
	void ScheduleNextSpawn();
	void DoSpawn();
	UFUNCTION()
	void HandleEnemyDied(AEnemyBase* Enemy);

	float GetCurrentInterval() const;
	bool FindSpawnLocation(FVector& OutLocation) const;

	UPROPERTY(EditAnywhere, Category="Spawning|Setup")
	TSubclassOf<AEnemyBase> EnemyClass;

	UPROPERTY(EditAnywhere, Category="Spawning|Setup")
	int32 MaxTotalSpawned = 50;

	// Optional: Actor implementing IActivatable to activate when all enemies are dead (e.g., BP_ExitGate)
	UPROPERTY(EditAnywhere, Category="Spawning|Setup")
	TObjectPtr<AActor> ExitGate = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Spawning|State")
	int32 TotalSpawned = 0;

	UPROPERTY(VisibleAnywhere, Category="Spawning|State")
	int32 AliveCount = 0;

	UPROPERTY(EditAnywhere, Category="Spawning|Distances")
	float MinDistanceFromPlayer = 1200.f;

	UPROPERTY(EditAnywhere, Category="Spawning|Distances")
	float MaxDistanceFromPlayer = 5000.f;

	UPROPERTY(EditAnywhere, Category="Spawning|Timing")
	float InitialInterval = 6.0f;

	UPROPERTY(EditAnywhere, Category="Spawning|Timing")
	float MinInterval = 1.0f;

	// Linear decrease: Interval = Initial - (minutes * DecreasePerMinute), clamped to MinInterval.
	UPROPERTY(EditAnywhere, Category="Spawning|Timing")
	float DecreasePerMinute = 0.8f;

	float StartTimeSeconds = 0.f;

	FTimerHandle SpawnHandle;
};
