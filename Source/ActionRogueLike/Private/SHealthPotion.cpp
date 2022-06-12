// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

#include "SAttributeComponent.h"

ASHealthPotion::ASHealthPotion()
{
	HealthAmount = 20.f;
	ReactiveDelay = 10.f;
}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{

	USAttributeComponent* TargetAC = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));

	if(TargetAC)
	{
		if(TargetAC->GetHealth() < TargetAC->GetMaxHealth())
		{
			TargetAC->ApplyHealthChange(HealthAmount);
			Super::Interact_Implementation(InstigatorPawn);
		}
	}
	
	
}
