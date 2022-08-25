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

	//LogOnScreen(this, FString::Printf(TEXT("started: %s"), *ActionName.ToString()), FColor::Green);
	
	GetOwningActionComponent()->ActiveGameplayTags.AppendTags(GrantsTags);
	RepData.bIsRunning = true;
	RepData.Instigator = Instigator;
	if(GetOwningActionComponent()->GetOwnerRole() == ROLE_Authority)
	{
		ActionStartTime = GetWorld()->GetTimeSeconds();
	}
	
	GetOwningActionComponent()->OnActionStarted.Broadcast(GetOwningActionComponent(), this);
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));
	//LogOnScreen(this, FString::Printf(TEXT("stopped: %s"), *ActionName.ToString()), FColor::White);

	//ensureAlways(bIsRunning);
	
	GetOwningActionComponent()->ActiveGameplayTags.RemoveTags(GrantsTags);
	RepData.bIsRunning = false;
	RepData.Instigator = Instigator;

	GetOwningActionComponent()->OnActionStopped.Broadcast(GetOwningActionComponent(), this);
}


bool USAction::CanStart_Implementation(AActor* Instigator)
{
	if(RepData.bIsRunning) return false;
	
	return !GetOwningActionComponent()->ActiveGameplayTags.HasAny(BlockedTags);
}

bool USAction::GetIsRunning() const
{
	return RepData.bIsRunning;
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

void USAction::OnRep_RepData()
{
	if(RepData.bIsRunning)
	{
		StartAction(RepData.Instigator);
	}

	else
	{
		StopAction(RepData.Instigator);
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
	DOREPLIFETIME(ThisClass, RepData);
	DOREPLIFETIME(ThisClass, ActionStartTime);
}
