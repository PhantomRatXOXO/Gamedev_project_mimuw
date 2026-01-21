#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class AEnemyBase;

UCLASS()
class GAMEDEVPROJECT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	void Think();

	UPROPERTY()
	TObjectPtr<AEnemyBase> ControlledEnemy = nullptr;

	UPROPERTY()
	TObjectPtr<APawn> PlayerPawn = nullptr;

	UPROPERTY(EditAnywhere, Category="AI")
	float ThinkInterval = 0.2f;

	UPROPERTY(EditAnywhere, Category="AI")
	float MoveAcceptanceRadius = 50.f;

	FTimerHandle ThinkHandle;
};
