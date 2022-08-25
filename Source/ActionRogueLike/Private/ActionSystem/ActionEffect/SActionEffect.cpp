// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/ActionEffect/SActionEffect.h"

#include "ActionSystem/SActionComponent.h"
#include "GameFramework/GameStateBase.h"

USActionEffect::USActionEffect()
{
	bAutoStart = true;
}

void USActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if(Duration > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StopAction", Instigator);

		GetWorld()->GetTimerManager().SetTimer(DurationHandle, Delegate, Duration, false);

		if(Period > 0.0f)
		{
			FTimerDelegate PeriodDelegate;
			PeriodDelegate.BindUFunction(this, "ExecutePeriodEffect", Instigator);
		
			GetWorld()->GetTimerManager().SetTimer(PeriodHandle, PeriodDelegate, Period, true);
		}
	}

	
}

void USActionEffect::StopAction_Implementation(AActor* Instigator)
{


	if(GetWorld()->GetTimerManager().GetTimerRemaining(PeriodHandle) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodEffect(Instigator);
	}
	
	Super::StopAction_Implementation(Instigator);
	
	GetWorld()->GetTimerManager().ClearTimer(PeriodHandle);
	GetWorld()->GetTimerManager().ClearTimer(DurationHandle);

	if(GetOwningActionComponent() != nullptr)
	{
		GetOwningActionComponent()->RemoveAction(this);
	}
}

float USActionEffect::GetTimeRemaining() const
{

	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();

	if(GS)
	{
		const float EndTime = ActionStartTime + Duration;

		return EndTime - GS->GetServerWorldTimeSeconds();
	}
	
	return Duration;
}


void USActionEffect::ExecutePeriodEffect_Implementation(AActor* Instigator)
{
	
}


