#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AEnemyAIController.generated.h"

UCLASS()
class GAMEDEVPROJECT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UPROPERTY(EditAnywhere, Category="AI")
	float ThinkInterval = 0.2f;

	FTimerHandle ThinkTimer;

	void Think();
};
