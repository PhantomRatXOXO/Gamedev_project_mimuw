#include "Characters/ABaseCharacter.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDead())
	{
		return 0.f;
	}

	const float Applied = FMath::Max(0.f, DamageAmount);
	CurrentHealth = FMath::Clamp(CurrentHealth - Applied, 0.f, MaxHealth);

	BP_OnHit(Applied);

	if (IsDead())
	{
		Die();
	}

	return Applied;
}

void ABaseCharacter::Die()
{
	BP_OnDeath();

	// Notify listeners (spawner etc.)
	OnDied.Broadcast(this);

	Destroy();
}
