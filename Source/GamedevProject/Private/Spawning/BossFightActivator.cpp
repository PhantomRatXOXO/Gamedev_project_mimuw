#include "Spawning/BossFightActivator.h"

#include "Components/BoxComponent.h"
#include "Enemies/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Spawning/EnemySpawner.h"

ABossFightActivator::ABossFightActivator()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);

	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);
}

void ABossFightActivator::BeginPlay()
{
	Super::BeginPlay();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ABossFightActivator::OnTriggerBeginOverlap);

	if (IsValid(SpawnerRef))
	{
		SpawnerRef->OnLevelCleared.AddDynamic(this, &ABossFightActivator::OnSpawnerCleared);
	}
}

void ABossFightActivator::OnSpawnerCleared()
{
	bBossFightEnabled = true;
}

void ABossFightActivator::OnBossDied(AEnemyBase* Enemy)
{
	if (!IsValid(Enemy) || Enemy != BossRef)
	{
		return;
	}

	OnBossDefeated.Broadcast();
	BossRef = nullptr;
}

bool ABossFightActivator::FindBossSpawnLocation(FVector& OutLocation) const
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(PlayerPawn))
	{
		return false;
	}

	UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!Nav)
	{
		return false;
	}

	const FVector PlayerLoc = PlayerPawn->GetActorLocation();

	for (int32 Attempt = 0; Attempt < 16; ++Attempt)
	{
		FNavLocation NavLoc;
		if (!Nav->GetRandomReachablePointInRadius(PlayerLoc, MaxBossDistanceFromPlayer, NavLoc))
		{
			continue;
		}

		const float Dist = FVector::Dist(PlayerLoc, NavLoc.Location);
		if (Dist < MinBossDistanceFromPlayer)
		{
			continue;
		}

		OutLocation = NavLoc.Location;
		return true;
	}

	return false;
}

void ABossFightActivator::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                               bool bFromSweep, const FHitResult& SweepResult)
{
	if (bBossSpawned || !bBossFightEnabled || !BossClass)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(PlayerPawn) || OtherActor != PlayerPawn)
	{
		return;
	}

	FVector SpawnLoc;
	if (!FindBossSpawnLocation(SpawnLoc))
	{
		return;
	}

	const FRotator SpawnRot(0.f, FMath::FRandRange(-180.f, 180.f), 0.f);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemyBase* Boss = GetWorld()->SpawnActor<AEnemyBase>(BossClass, SpawnLoc, SpawnRot, Params);
	if (Boss)
	{
		bBossSpawned = true;
		BossRef = Boss;
		Boss->OnEnemyDied.AddDynamic(this, &ABossFightActivator::OnBossDied);
	}
}
