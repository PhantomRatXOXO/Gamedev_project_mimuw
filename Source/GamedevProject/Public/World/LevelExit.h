#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelExit.generated.h"

class UBoxComponent;
class AEnemySpawner;
class ABossFightActivator;
class UUserWidget;

UENUM(BlueprintType)
enum class ELevelExitAction : uint8
{
	OpenLevel,
	ShowVictoryWidget
};

UCLASS()
class GAMEDEVPROJECT_API ALevelExit : public AActor
{
	GENERATED_BODY()

public:
	ALevelExit();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							  bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleSpawnerCleared();

	UFUNCTION()
	void HandleBossDefeated();

	void TryOpen();
	void OpenExit();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Trigger = nullptr;

	UPROPERTY(EditAnywhere, Category="Objective")
	TObjectPtr<AEnemySpawner> SpawnerRef = nullptr;

	UPROPERTY(EditAnywhere, Category="Objective")
	TObjectPtr<ABossFightActivator> BossActivatorRef = nullptr;

	UPROPERTY(EditAnywhere, Category="Objective")
	bool bRequireBossDefeated = false;

	UPROPERTY(EditAnywhere, Category="Gate")
	TObjectPtr<AActor> GateActor = nullptr;

	UPROPERTY(EditAnywhere, Category="Gate")
	bool bDisableGateCollisionOnOpen = true;

	UPROPERTY(EditAnywhere, Category="Gate")
	bool bHideGateOnOpen = false;

	UPROPERTY(EditAnywhere, Category="Exit")
	ELevelExitAction ExitAction = ELevelExitAction::OpenLevel;

	UPROPERTY(EditAnywhere, Category="Exit", meta=(EditCondition="ExitAction==ELevelExitAction::OpenLevel"))
	FName NextLevelName;

	UPROPERTY(EditAnywhere, Category="Exit", meta=(EditCondition="ExitAction==ELevelExitAction::ShowVictoryWidget"))
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	UPROPERTY(EditAnywhere, Category="Exit", meta=(EditCondition="ExitAction==ELevelExitAction::ShowVictoryWidget"))
	bool bPauseGameOnWin = true;

	bool bSpawnerCleared = false;
	bool bBossDefeated = false;
	bool bIsOpen = false;
};
