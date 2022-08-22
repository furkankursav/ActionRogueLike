// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_Action.h"

#include "ActionSystem/SActionComponent.h"


// Sets default values
ASPowerup_Action::ASPowerup_Action()
{
	
}


void ASPowerup_Action::Interact_Implementation(APawn* InstigatorPawn)
{
	if(!ensure(InstigatorPawn && ActionToGrant))
	{
		return;
	}

	USActionComponent* ActionComp = InstigatorPawn->FindComponentByClass<USActionComponent>();

	if(ActionComp)
	{
		if(ActionComp->GetActionByClass(ActionToGrant))
		{
			return;
		}

		ActionComp->AddAction(InstigatorPawn, ActionToGrant);
		HideAndCoolDownPowerup();
	}
}

