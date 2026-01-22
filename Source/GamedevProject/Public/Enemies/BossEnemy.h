#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "BossEnemy.generated.h"

class UHealthBarWidget;
class UUserWidget;

UCLASS()
class GAMEDEVPROJECT_API ABossEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	ABossEnemy();

protected:
	virtual void BeginPlay() override;

	virtual void HandleHealthChanged(float CurrentHealth, float InMaxHealth) override;

	// Widget class for the boss health bar (create a WBP_BossHealthBar in Editor)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|UI")
	TSubclassOf<UUserWidget> BossHealthBarClass;

private:
	UPROPERTY()
	TObjectPtr<UHealthBarWidget> BossHealthBar = nullptr;
};
