// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	MaxHealth = 100.f;
	Health = MaxHealth;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return (Health == MaxHealth);
}

bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	const float OldHealth = Health;
	
	Health = FMath::Clamp(Health + Delta, 0, MaxHealth);

	const float ActualDelta = Health - OldHealth;
	
	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);
	
	return ActualDelta != 0;
}

