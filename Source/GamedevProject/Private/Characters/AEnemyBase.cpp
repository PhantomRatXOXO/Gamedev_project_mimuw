#include "Characters/AEnemyBase.h"

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "AI/AEnemyAIController.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyBase::DealDamage(AActor* Target)
{
	if (!Target || IsDead())
	{
		return;
	}

	UGameplayStatics::ApplyDamage(
		Target,
		Damage,
		GetController(),
		this,
		nullptr
	);
}

bool AEnemyBase::TryAttack(AActor* Target)
{
	if (!Target || IsDead() || !bCanAttack)
	{
		return false;
	}

	const float Dist = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	if (Dist > AttackRange)
	{
		return false;
	}

	bCanAttack = false;

	// For now: immediate damage. Later you can sync via montage notify.
	DealDamage(Target);

	// Cooldown reset
	GetWorldTimerManager().SetTimer(
		AttackCooldownTimer,
		[this]() { bCanAttack = true; },
		AttackCooldown,
		false
	);

	return true;
}

void AEnemyBase::Die()
{
	// Enemy-specific additions can go here before destruction.
	// Example: stop AI movement, drop loot, etc.

	// Call base: BP_OnDeath + OnDied + Destroy
	Super::Die();
}
