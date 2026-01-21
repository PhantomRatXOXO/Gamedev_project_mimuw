#include "Characters/AParasiteEnemy.h"

AParasiteEnemy::AParasiteEnemy()
{
	// Reasonable starting defaults; tune in BP child defaults per level
	MaxHealth = 40.f;
	// CurrentHealth will be set from MaxHealth in BeginPlay of base

	AttackRange = 140.f;
	Damage = 8.f;
	AttackCooldown = 1.0f;
}
