// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/Actions/SAction.h"

#include "ActionRogueLike/ActionRogueLike.h"
#include "ActionSystem/SActionComponent.h"
#include "Net/UnrealNetwork.h"


void USAction::Initialize(USActionComponent* NewActionComp)
{
	ActionComp = NewActionComp;
}

void USAction::StartAction_Implementation(AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));

	LogOnScreen(this, FString::Printf(TEXT("started: %s"), *ActionName.ToString()), FColor::Green);
	
	GetOwningActionComponent()->ActiveGameplayTags.AppendTags(GrantsTags);
	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));
	LogOnScreen(this, FString::Printf(TEXT("stopped: %s"), *ActionName.ToString()), FColor::White);

	//ensureAlways(bIsRunning);
	
	GetOwningActionComponent()->ActiveGameplayTags.RemoveTags(GrantsTags);
	bIsRunning = false;
}


bool USAction::CanStart_Implementation(AActor* Instigator)
{
	if(bIsRunning) return false;
	
	return !GetOwningActionComponent()->ActiveGameplayTags.HasAny(BlockedTags);
}

bool USAction::GetIsRunning() const
{
	return bIsRunning;
}

UWorld* USAction::GetWorld() const
{
	// Outer is set when creating action via NewObject<T>()
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if(Comp)
	{
		return Comp->GetWorld();
	}

	return nullptr;
}

void USAction::OnRep_IsRunning()
{
	if(bIsRunning)
	{
		StartAction(nullptr);
	}

	else
	{
		StopAction(nullptr);
	}
}

USActionComponent* USAction::GetOwningActionComponent() const
{
	return Cast<USActionComponent>(GetOuter());
}



void USAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, ActionComp);
	DOREPLIFETIME(ThisClass, bIsRunning);
}
