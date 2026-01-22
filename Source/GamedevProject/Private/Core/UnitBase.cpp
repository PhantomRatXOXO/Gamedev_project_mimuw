#include "Core/UnitBase.h"

AUnitBase::AUnitBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AUnitBase::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}

float AUnitBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return 0.f;
	}

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - ActualDamage, 0.f, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("%s took %.1f damage. Health: %.1f/%.1f"),
		*GetName(), ActualDamage, Health, MaxHealth);

	if (!IsAlive())
	{
		Die();
	}

	return ActualDamage;
}

void AUnitBase::Die()
{
	UE_LOG(LogTemp, Log, TEXT("%s died."), *GetName());
	OnUnitDied.Broadcast(this);
}
