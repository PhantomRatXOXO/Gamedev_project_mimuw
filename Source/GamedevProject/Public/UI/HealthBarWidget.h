#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UProgressBar;

UCLASS()
class GAMEDEVPROJECT_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetHealthPercent(float InPercent);

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar = nullptr;

private:
	float CachedPercent = 1.f;
};
