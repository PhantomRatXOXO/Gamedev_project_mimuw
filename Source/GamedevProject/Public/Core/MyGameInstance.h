// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

UCLASS()
class GAMEDEVPROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Called once when the game starts (persists across level loads)
	virtual void Init() override;

	// Example persistent state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Progress")
	int32 CurrentLevelIndex = 0;

	// Example: store where player should spawn after OpenLevel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	FName PendingSpawnTag = NAME_None;

	// Convenience function callable from Blueprints
	UFUNCTION(BlueprintCallable, Category="Spawn")
	void SetPendingSpawnTag(FName NewTag);

	// Convenience function callable from Blueprints
	UFUNCTION(BlueprintCallable, Category="Progress")
	void SetCurrentLevelIndex(int32 NewIndex);
};
