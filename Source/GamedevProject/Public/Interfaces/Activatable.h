#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Activatable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UActivatable : public UInterface
{
	GENERATED_BODY()
};

class GAMEDEVPROJECT_API IActivatable
{
	GENERATED_BODY()

public:
	// Called to activate the object (portal, door, etc.)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Activation")
	void Activate();
};
