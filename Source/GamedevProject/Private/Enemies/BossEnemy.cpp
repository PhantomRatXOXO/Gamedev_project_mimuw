#include "Enemies/BossEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"

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
