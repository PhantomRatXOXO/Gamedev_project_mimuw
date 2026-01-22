#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return 0.f;
	}

	const float Applied = FMath::Max(0.f, DamageAmount);
	if (Applied <= 0.f)
	{
		return 0.f;
	}

	Health = FMath::Max(0.f, Health - Applied);
	OnHealthChanged.Broadcast(Health, MaxHealth);

	if (Health <= 0.f)
	{
		Die();
	}

	return Applied;
}

void ABaseCharacter::Die_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetName());
	// Stop movement
	if (AController* C = GetController())
	{
		C->StopMovement();
	}

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->DisableMovement();
	}

	// Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Play death animation if assigned
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	// Notify listeners
	OnCharacterDied.Broadcast(this);
}

void ABaseCharacter::HandleHealthChanged(float CurrentHealth, float InMaxHealth)
{
	// Base implementation does nothing - subclasses override for UI updates
}
