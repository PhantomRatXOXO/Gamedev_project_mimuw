#include "AI/EnemyAIController.h"

#include "Enemies/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AEnemyAIController::AEnemyAIController()
{
	bAttachToPawn = true;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledEnemy = Cast<AEnemyBase>(InPawn);
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	GetWorldTimerManager().ClearTimer(ThinkHandle);
	GetWorldTimerManager().SetTimer(
		ThinkHandle,
		this,
		&AEnemyAIController::Think,
		ThinkInterval,
		true
	);
}

void AEnemyAIController::OnUnPossess()
{
	GetWorldTimerManager().ClearTimer(ThinkHandle);
	ControlledEnemy = nullptr;
	PlayerPawn = nullptr;

	Super::OnUnPossess();
}

void AEnemyAIController::Think()
{
	if (!IsValid(ControlledEnemy) || !ControlledEnemy->IsAlive())
	{
		StopMovement();
		return;
	}

	if (ControlledEnemy->IsAttacking())
	{
		StopMovement();
		return;
	}

	if (!IsValid(PlayerPawn))
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (!IsValid(PlayerPawn))
		{
			return;
		}
	}

	const float Dist = FVector::Dist2D(ControlledEnemy->GetActorLocation(), PlayerPawn->GetActorLocation());
	const float AttackRange = ControlledEnemy->GetAttackRange();

	if (Dist > AttackRange)
	{
		ClearFocus(EAIFocusPriority::Gameplay);
		MoveToActor(PlayerPawn, MoveAcceptanceRadius, true, true, false, 0, true);
	}
	else
	{
		StopMovement();
		SetFocus(PlayerPawn);
		ControlledEnemy->TryAttack(PlayerPawn);
	}
}
