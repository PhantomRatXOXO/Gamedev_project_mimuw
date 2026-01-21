#pragma once

#include "CoreMinimal.h"
#include "Characters/ABaseCharacter.h"
#include "AMainCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GAMEDEVPROJECT_API AMainCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// Optional
	virtual void Die() override;

	UFUNCTION(BlueprintImplementableEvent, Category="VFX")
	void BP_OnPlayerDeath();
};
