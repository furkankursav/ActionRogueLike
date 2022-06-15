// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

#include "SAttributeComponent.h"

ASHealthPotion::ASHealthPotion()
{
	HealthAmount = 20.f;
}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{


	if(!ensure(InstigatorPawn))
	{
		return;
	}
	
	USAttributeComponent* TargetAC =USAttributeComponent::GetAttributes(InstigatorPawn);

	if(TargetAC && TargetAC->IsFullHealth() == false)
	{
		if(TargetAC->ApplyHealthChange(this, HealthAmount))
		{
			HideAndCoolDownPowerup();
		}
	}
	
}
