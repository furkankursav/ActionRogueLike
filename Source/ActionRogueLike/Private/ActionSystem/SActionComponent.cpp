// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/SActionComponent.h"

#include "ActionRogueLike/ActionRogueLike.h"
#include "ActionSystem/Actions/SAction.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"


USActionComponent::USActionComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}



void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwner()->HasAuthority())
	{
		for(TSubclassOf<USAction> const ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}
	
	
}




void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// const FString DebugMessage = GetNameSafe(GetOwner()) + ": " +ActiveGameplayTags.ToStringSimple();
	// GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMessage);

	// Write all actions
	for(USAction* Action : Actions)
	{
		if(Action != nullptr)
		{
			const FColor TextColor = Action->GetIsRunning() ? FColor::Blue : FColor::White;

			FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s : IsRunning: %s : Outer: %s"),
				*GetNameSafe(GetOwner()),
					*Action->ActionName.ToString(),
					Action->GetIsRunning() ? TEXT("true") : TEXT("false"),
					*GetNameSafe(GetOuter()));

			LogOnScreen(this, ActionMsg, TextColor, 0.0f);
		}
	}
	
}

USAction* USActionComponent::GetActionByClass(TSubclassOf<USAction> ActionClass) const
{
	if(IsValid(ActionClass) == false) return nullptr;
	
	for(USAction* Action : Actions)
	{
		if(Action && Action->IsA(ActionClass))
		{
			return Action;
		}
	}

	return nullptr;
}



void USActionComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass)
{
	if(!ensure(ActionClass))
	{
		return;
	}

	USAction* NewAction = NewObject<USAction>(this, ActionClass);

	if(ensure(NewAction))
	{
		NewAction->Initialize(this);
		Actions.Add(NewAction);

		if(NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
}

void USActionComponent::RemoveAction(USAction* ActionToRemove)
{
	if(!ensure(ActionToRemove && ActionToRemove->GetIsRunning() == false))
	{
		return;
	}
	
	Actions.Remove(ActionToRemove);

}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for(USAction* Action : Actions)
	{
		if(Action && Action->ActionName == ActionName)
		{
			if(Action->CanStart(Instigator) == false)
			{
				FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FailedMsg);
				continue;
			}

			// Is Client?
			if(!GetOwner()->HasAuthority())
			{
				ServerStartAction(Instigator, ActionName);
			}
			
			Action->StartAction(Instigator);
			return true;
		}
	}

	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for(USAction* Action : Actions)
	{
		if(Action && Action->ActionName == ActionName)
		{
			if(Action->GetIsRunning())
			{
				Action->StopAction(Instigator);
				return true;
			}
			
		}
	}

	return false;
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}

void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, Actions);
}

bool USActionComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bSuperReplicates = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for(USAction* Action : Actions)
	{
		if(Action)
		{
			bSuperReplicates |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
		}
	}

	return bSuperReplicates;
}