// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/Actions/SAction.h"
#include "ActionSystem/SActionComponent.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));
	
	GetOwningActionComponent()->ActiveGameplayTags.AppendTags(GrantsTags);
	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));

	ensureAlways(bIsRunning);
	
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

USActionComponent* USAction::GetOwningActionComponent() const
{
	return Cast<USActionComponent>(GetOuter());
}