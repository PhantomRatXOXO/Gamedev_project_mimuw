#include "Spawning/AParasiteSpawner.h"

#include "Characters/AEnemyBase.h"
#include "Characters/ABaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "TimerManager.h"

AParasiteSpawner::AParasiteSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AParasiteSpawner::BeginPlay()
{
	Super::BeginPlay();

	StartTime = GetWorld()->GetTimeSeconds();
	ScheduleNextSpawn();
}

bool AParasiteSpawner::IsCleared() const
{
	return (TotalSpawned >= MaxTotalSpawned) && (AliveCount <= 0);
}

float AParasiteSpawner::GetCurrentInterval() const
{
	const float Now = GetWorld()->GetTimeSeconds();
	const float ElapsedMinutes = (Now - StartTime) / 60.f;

	const float Interval = InitialInterval - (ElapsedMinutes * IntervalDecreasePerMinute);
	return FMath::Clamp(Interval, MinInterval, InitialInterval);
}

void AParasiteSpawner::ScheduleNextSpawn()
{
	if (!ParasiteClass) return;
	if (TotalSpawned >= MaxTotalSpawned) return;

	const float Delay = GetCurrentInterval();
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AParasiteSpawner::DoSpawn, Delay, false);
}

void AParasiteSpawner::DoSpawn()
{
	if (!ParasiteClass) return;
	if (TotalSpawned >= MaxTotalSpawned) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn) { ScheduleNextSpawn(); return; }

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) { ScheduleNextSpawn(); return; }

	const FVector PlayerLoc = PlayerPawn->GetActorLocation();
	FNavLocation Candidate;

	bool bFound = false;
	for (int32 Attempt = 0; Attempt < 14; ++Attempt)
	{
		if (NavSys->GetRandomReachablePointInRadius(PlayerLoc, MaxDistanceFromPlayer, Candidate))
		{
			const float Dist = FVector::Dist(PlayerLoc, Candidate.Location);
			if (Dist >= MinDistanceFromPlayer)
			{
				bFound = true;
				break;
			}
		}
	}

	if (!bFound)
	{
		// Try again later; don't consume quota
		ScheduleNextSpawn();
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ParasiteClass, Candidate.Location, FRotator::ZeroRotator, Params);
	if (Spawned)
	{
		TotalSpawned++;
		AliveCount++;

		// Bind to death event if it is a BaseCharacter
		if (ABaseCharacter* BC = Cast<ABaseCharacter>(Spawned))
		{
			BC->OnDied.AddDynamic(this, &AParasiteSpawner::HandleSpawnedDied);
		}
	}

	ScheduleNextSpawn();
}

void AParasiteSpawner::HandleSpawnedDied(AActor* DeadActor)
{
	AliveCount = FMath::Max(0, AliveCount - 1);

	if (IsCleared())
	{
		OnLevelCleared.Broadcast();
	}
}
