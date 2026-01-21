#pragma once

#include "CoreMinimal.h"
#include "Characters/AEnemyBase.h"
#include "ABossEnemy.generated.h"

UCLASS(Blueprintable)
class GAMEDEVPROJECT_API ABossEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	ABossEnemy();
};
