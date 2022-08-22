// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"
#include "Net/UnrealNetwork.h"
#include "SGameModeBase.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component"), ECVF_Cheat);

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	MaxHealth = 100.f;
	Health = MaxHealth;

	SetIsReplicatedByDefault(true);
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if(FromActor)
	{
		return FromActor->FindComponentByClass<USAttributeComponent>();
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	USAttributeComponent* TargetAC = GetAttributes(Actor);

	if(TargetAC)
	{
		return TargetAC->IsAlive();
	}

	return false;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetMaxHealth());
}

bool USAttributeComponent::IsFullHealth() const
{
	return (Health == MaxHealth);
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{

	if(GetOwner()->CanBeDamaged() == false && Delta < 0.0f) return false;

	if(Delta < 0.0f)
	{
		const float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();
		Delta *= DamageMultiplier;
	}
	
	const float OldHealth = Health;
	
	Health = FMath::Clamp(Health + Delta, 0, MaxHealth);

	const float ActualDelta = Health - OldHealth;
	if(ActualDelta != 0.f)
	{
		MulticastHealthChanged(InstigatorActor, this, Health, Delta);
	}
	//OnHealthChanged.Broadcast(InstigatorActor, this, Health, Delta);
	// Died
	if(ActualDelta < 0.f && Health <= 0.f)
	{
		ASGameModeBase* GMBase = GetWorld()->GetAuthGameMode<ASGameModeBase>();

		if(GMBase)
		{
			GMBase->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}
	
	return ActualDelta != 0;
}

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, OwningComp, NewHealth, Delta);
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MaxHealth);
	DOREPLIFETIME(ThisClass, Health);
}
