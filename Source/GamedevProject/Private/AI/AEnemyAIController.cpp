#include "AI/AEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/AEnemyBase.h"
#include "TimerManager.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	GetWorldTimerManager().SetTimer(
		ThinkTimer, this, &AEnemyAIController::Think, ThinkInterval, true
	);
}

void AEnemyAIController::OnUnPossess()
{
	GetWorldTimerManager().ClearTimer(ThinkTimer);
	Super::OnUnPossess();
}

void AEnemyAIController::Think()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn) return;

	AEnemyBase* Enemy = Cast<AEnemyBase>(GetPawn());
	if (!Enemy || Enemy->IsDead()) return;

	// Pathfind toward player; AcceptanceRadius slightly below attack range
	const float Acceptance = FMath::Max(50.f, Enemy->GetAttackRange() - 25.f);
	MoveToActor(PlayerPawn, Acceptance);

	// Attempt attack (your TryAttack already checks distance/cooldown)
	Enemy->TryAttack(PlayerPawn);
}
