#include "Spawning/ABossActivator.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Spawning/AParasiteSpawner.h"
#include "Characters/AEnemyBase.h"

ABossActivator::ABossActivator()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);

	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);
}

void ABossActivator::BeginPlay()
{
	Super::BeginPlay();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ABossActivator::OnTriggerBeginOverlap);

	if (SpawnerRef)
	{
		if (SpawnerRef->IsCleared())
		{
			bBossFightEnabled = true;
		}
		else
		{
			SpawnerRef->OnLevelCleared.AddDynamic(this, &ABossActivator::HandleLevelCleared);
		}
	}
}

void ABossActivator::HandleLevelCleared()
{
	bBossFightEnabled = true;
}

void ABossActivator::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!bBossFightEnabled || bBossSpawned || !BossClass)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (OtherActor != PlayerPawn)
	{
		return;
	}

	SpawnBoss();
}

void ABossActivator::SpawnBoss()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn) return;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return;

	const FVector PlayerLoc = PlayerPawn->GetActorLocation();
	FNavLocation Candidate;

	bool bFound = false;
	for (int32 Attempt = 0; Attempt < 24; ++Attempt)
	{
		if (NavSys->GetRandomReachablePointInRadius(PlayerLoc, MaxBossDistanceFromPlayer, Candidate))
		{
			const float Dist = FVector::Dist(PlayerLoc, Candidate.Location);
			if (Dist >= MinBossDistanceFromPlayer)
			{
				bFound = true;
				break;
			}
		}
	}

	if (!bFound) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(BossClass, Candidate.Location, FRotator::ZeroRotator, Params);
	bBossSpawned = true;
}
