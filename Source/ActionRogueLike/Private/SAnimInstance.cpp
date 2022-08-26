// Fill out your copyright notice in the Description page of Project Settings.


#include "SAnimInstance.h"

#include "GameplayTagContainer.h"
#include "ActionSystem/SActionComponent.h"

static FGameplayTag StunnedTag = FGameplayTag::RequestGameplayTag("Status.Stunned");

void USAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if(const AActor* OwningActor = GetOwningActor())
	{
		ActionComp = OwningActor->FindComponentByClass<USActionComponent>();
	}
}

void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);


	if(ActionComp)
	{
		bIsStunned = ActionComp->ActiveGameplayTags.HasTag(StunnedTag);
	}
	
	
}


