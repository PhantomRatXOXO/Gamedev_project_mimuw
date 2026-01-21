// Fill out your copyright notice in the Description page of Project Settings. 

#include "Core/MyGameInstance.h"
#include "Engine/Engine.h"

void UMyGameInstance::Init()
{
	Super::Init();

	// Debug message to confirm it's active
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.0f, FColor::Green,
			TEXT("UMyGameInstance::Init() - GameInstance is running")
		);
	}
}

void UMyGameInstance::SetPendingSpawnTag(FName NewTag)
{
	PendingSpawnTag = NewTag;
}

void UMyGameInstance::SetCurrentLevelIndex(int32 NewIndex)
{
	CurrentLevelIndex = NewIndex;
}
