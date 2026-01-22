#include "Spawning/EnemySpawner.h"

#include "Enemies/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "TimerManager.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	StartTimeSeconds = GetWorld()->GetTimeSeconds();
	UE_LOG(LogTemp, Warning, TEXT("EnemySpawner BeginPlay: EnemyClass=%s MaxTotalSpawned=%d"),
		EnemyClass ? *EnemyClass->GetName() : TEXT("None"),
		MaxTotalSpawned);
	ScheduleNextSpawn();
}

float AEnemySpawner::GetCurrentInterval() const
{
	const float Now = GetWorld()->GetTimeSeconds();
	const float Minutes = (Now - StartTimeSeconds) / 60.f;

	const float Interval = InitialInterval - Minutes * DecreasePerMinute;
	return FMath::Clamp(Interval, MinInterval, InitialInterval);
}

void AEnemySpawner::ScheduleNextSpawn()
{
	if (TotalSpawned >= MaxTotalSpawned || !EnemyClass)
	{
		return;
	}

	const float Interval = GetCurrentInterval();
	GetWorldTimerManager().ClearTimer(SpawnHandle);
	GetWorldTimerManager().SetTimer(SpawnHandle, this, &AEnemySpawner::DoSpawn, Interval, false);
}

bool AEnemySpawner::FindSpawnLocation(FVector& OutLocation) const
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(PlayerPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemySpawner FindSpawnLocation: No valid PlayerPawn"));
		return false;
	}

	UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!Nav)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemySpawner FindSpawnLocation: No NavSystem"));
		return false;
	}

	const FVector PlayerLoc = PlayerPawn->GetActorLocation();

	for (int32 Attempt = 0; Attempt < 12; ++Attempt)
	{
		FNavLocation NavLoc;
		const bool bOk = Nav->GetRandomReachablePointInRadius(PlayerLoc, MaxDistanceFromPlayer, NavLoc);
		if (!bOk)
		{
			UE_LOG(LogTemp, Warning, TEXT("EnemySpawner FindSpawnLocation: No reachable point (attempt %d)"), Attempt + 1);
			continue;
		}

		const float Dist = FVector::Dist(PlayerLoc, NavLoc.Location);
		if (Dist < MinDistanceFromPlayer)
		{
			UE_LOG(LogTemp, Warning, TEXT("EnemySpawner FindSpawnLocation: Too close (%.1f < %.1f)"), Dist, MinDistanceFromPlayer);
			continue;
		}

		OutLocation = NavLoc.Location;
		return true;
	}

	return false;
}

void AEnemySpawner::DoSpawn()
{
	if (TotalSpawned >= MaxTotalSpawned || !EnemyClass)
	{
		return;
	}

	FVector SpawnLoc;
	if (!FindSpawnLocation(SpawnLoc))
	{
		// If nav returns nothing, try again later.
		ScheduleNextSpawn();
		return;
	}

	const FRotator SpawnRot = FRotator(0.f, FMath::FRandRange(-180.f, 180.f), 0.f);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	AEnemyBase* Spawned = GetWorld()->SpawnActor<AEnemyBase>(EnemyClass, SpawnLoc, SpawnRot, Params);
	if (!Spawned)
	{
		ScheduleNextSpawn();
		return;
	}

	TotalSpawned++;
	AliveCount++;

	Spawned->OnEnemyDied.AddDynamic(this, &AEnemySpawner::HandleEnemyDied);

	ScheduleNextSpawn();
}

void AEnemySpawner::HandleEnemyDied(AEnemyBase* Enemy)
{
	AliveCount = FMath::Max(0, AliveCount - 1);

	if (TotalSpawned >= MaxTotalSpawned && AliveCount == 0)
	{
		OnLevelCleared.Broadcast();
	}
}
