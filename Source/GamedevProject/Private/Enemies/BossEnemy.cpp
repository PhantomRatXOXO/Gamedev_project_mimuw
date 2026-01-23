#include "Enemies/BossEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/HealthBarWidget.h"
#include "Components/WidgetComponent.h"

ABossEnemy::ABossEnemy()
{
	MaxHealth = 500.f;
	Health = MaxHealth;
	Damage = 30.f;
	AttackRange = 250.f;
	AttackCooldown = 1.5f;
	AttackDamageDelay = 0.30f;
	AttackHitRadius = 200.f;

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = 220.f;
	}
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Hide the default floating health bar (inherited from EnemyBase)
	if (HealthWidgetComponent)
	{
		HealthWidgetComponent->SetVisibility(false);
	}

	// Rebind health changed to boss's override (parent bound to AEnemyBase::HandleHealthChanged)
	OnHealthChanged.Clear();
	OnHealthChanged.AddDynamic(this, &ABossEnemy::HandleHealthChanged);

	// Create and add boss health bar to viewport
	if (BossHealthBarClass)
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			BossHealthBar = CreateWidget<UHealthBarWidget>(PC, BossHealthBarClass);
			if (BossHealthBar)
			{
				BossHealthBar->AddToViewport();
				BossHealthBar->SetHealthPercent(GetHealthPercent());
			}
		}
	}
}

void ABossEnemy::HandleHealthChanged(float CurrentHealth, float InMaxHealth)
{
	// Update boss health bar instead of floating one
	if (BossHealthBar)
	{
		const float Percent = InMaxHealth > 0.f ? (CurrentHealth / InMaxHealth) : 0.f;
		BossHealthBar->SetHealthPercent(Percent);
	}
}
