// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

#include "SAttributeComponent.h"
#include "SPlayerState.h"

ASHealthPotion::ASHealthPotion()
{
	HealthAmount = 20.f;
	CreditCost = 50;
}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{


	if(!ensure(InstigatorPawn))
	{
		return;
	}
	
	USAttributeComponent* TargetAC = USAttributeComponent::GetAttributes(InstigatorPawn);

	if(TargetAC && TargetAC->IsFullHealth() == false)
	{

		if(ASPlayerState* PS = InstigatorPawn->GetPlayerState<ASPlayerState>())
		{
			if(PS->RemoveCredits(CreditCost) && TargetAC->ApplyHealthChange(this, HealthAmount))
			{
				HideAndCoolDownPowerup();
			}
		}
	}
	
}
