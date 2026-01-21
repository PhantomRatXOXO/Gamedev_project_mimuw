#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossFightActivator.generated.h"

class UBoxComponent;
class AEnemySpawner;
class AEnemyBase;

UCLASS()
class GAMEDEVPROJECT_API ABossFightActivator : public AActor
{
	GENERATED_BODY()

public:
	ABossFightActivator();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							  bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSpawnerCleared();

	bool FindBossSpawnLocation(FVector& OutLocation) const;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Trigger = nullptr;

	UPROPERTY(EditAnywhere, Category="Boss|Setup")
	TSubclassOf<AEnemyBase> BossClass;

	UPROPERTY(EditAnywhere, Category="Boss|Setup")
	TObjectPtr<AEnemySpawner> SpawnerRef = nullptr;

	UPROPERTY(EditAnywhere, Category="Boss|Distances")
	float MinBossDistanceFromPlayer = 2500.f;

	UPROPERTY(EditAnywhere, Category="Boss|Distances")
	float MaxBossDistanceFromPlayer = 8000.f;

	bool bBossFightEnabled = false;
	bool bBossSpawned = false;
};
