// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/ActionEffect/SActionEffect_Thorns.h"

#include "SGameplayFunctionLibrary.h"
#include "ActionSystem/SActionComponent.h"

USActionEffect_Thorns::USActionEffect_Thorns()
{
	ReflectFraction = 0.2f;

	Duration = 0.f;
	Period = 0.f;
}

void USActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if(IsValid(GetOwningActionComponent()->GetOwner()))
	{
		USAttributeComponent* Attributes = USAttributeComponent::GetAttributes(GetOwningActionComponent()->GetOwner());

		if(Attributes)
		{
			Attributes->OnHealthChanged.AddDynamic(this, &ThisClass::OnHealthChanged);
		}
	}
	
}

void USActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
	
	USAttributeComponent* Attributes = USAttributeComponent::GetAttributes(GetOwningActionComponent()->GetOwner());

	if(Attributes)
	{
		Attributes->OnHealthChanged.RemoveDynamic(this, &ThisClass::OnHealthChanged);
	}
}

void USActionEffect_Thorns::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	AActor* OwningActor = GetOwningActionComponent()->GetOwner();

	if(Delta < 0.0f && OwningActor != InstigatorActor)
	{
		 int32 ReflectedAmount = FMath::RoundToInt(Delta * ReflectFraction);
		
		if(ReflectedAmount == 0) return;

		ReflectedAmount = FMath::Abs(ReflectedAmount);

		USGameplayFunctionLibrary::ApplyDamage(OwningActor, InstigatorActor, ReflectedAmount);
	}
}
