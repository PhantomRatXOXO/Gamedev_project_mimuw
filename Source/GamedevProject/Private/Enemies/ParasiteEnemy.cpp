#include "Enemies/ParasiteEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"

AParasiteEnemy::AParasiteEnemy()
{
	MaxHealth = 50.f;
	Health = MaxHealth;
	Damage = 10.f;
	AttackRange = 150.f;
	AttackCooldown = 1.2f;
	AttackDamageDelay = 0.22f;
	AttackHitRadius = 120.f;

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = 260.f;
	}
}
