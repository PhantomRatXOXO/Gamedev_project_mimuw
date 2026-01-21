#include "World/LevelExit.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Spawning/EnemySpawner.h"
#include "Spawning/BossFightActivator.h"

ALevelExit::ALevelExit()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);

	Trigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(false);
	Trigger->SetHiddenInGame(true);
}

void ALevelExit::BeginPlay()
{
	Super::BeginPlay();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ALevelExit::OnTriggerBeginOverlap);

	if (IsValid(SpawnerRef))
	{
		SpawnerRef->OnLevelCleared.AddDynamic(this, &ALevelExit::HandleSpawnerCleared);
	}
	else
	{
		bSpawnerCleared = true;
	}

	if (bRequireBossDefeated)
	{
		if (IsValid(BossActivatorRef))
		{
			BossActivatorRef->OnBossDefeated.AddDynamic(this, &ALevelExit::HandleBossDefeated);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LevelExit: Boss required but BossActivatorRef is not set."));
		}
	}
	else
	{
		bBossDefeated = true;
	}

	TryOpen();
}

void ALevelExit::HandleSpawnerCleared()
{
	bSpawnerCleared = true;
	TryOpen();
}

void ALevelExit::HandleBossDefeated()
{
	bBossDefeated = true;
	TryOpen();
}

void ALevelExit::TryOpen()
{
	if (bIsOpen || !bSpawnerCleared || !bBossDefeated)
	{
		return;
	}

	OpenExit();
}

void ALevelExit::OpenExit()
{
	bIsOpen = true;

	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetGenerateOverlapEvents(true);

	if (IsValid(GateActor))
	{
		if (bDisableGateCollisionOnOpen)
		{
			GateActor->SetActorEnableCollision(false);
		}
		if (bHideGateOnOpen)
		{
			GateActor->SetActorHiddenInGame(true);
		}
	}
}

void ALevelExit::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
									  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
									  bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsOpen)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(PlayerPawn) || OtherActor != PlayerPawn)
	{
		return;
	}

	if (ExitAction == ELevelExitAction::OpenLevel)
	{
		if (NextLevelName.IsNone())
		{
			UE_LOG(LogTemp, Warning, TEXT("LevelExit: NextLevelName is not set."));
			return;
		}

		UGameplayStatics::OpenLevel(this, NextLevelName);
		return;
	}

	if (ExitAction == ELevelExitAction::ShowVictoryWidget)
	{
		if (!VictoryWidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("LevelExit: VictoryWidgetClass is not set."));
			return;
		}

		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			if (UUserWidget* Widget = CreateWidget<UUserWidget>(PC, VictoryWidgetClass))
			{
				Widget->AddToViewport();
			}

			if (bPauseGameOnWin)
			{
				PC->SetPause(true);
			}

			PC->bShowMouseCursor = true;
			FInputModeUIOnly InputMode;
			PC->SetInputMode(InputMode);
		}
	}
}
