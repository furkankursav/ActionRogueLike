// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "SSaveGame.h"
#include "Net/UnrealNetwork.h"

ASPlayerState::ASPlayerState()
{
	SetReplicates(true);
}

void ASPlayerState::AddCredits(const int32 Delta)
{
	// Avoid user error of adding a negative amount or zero
	if(Delta < 0)
	{
		return;
	}
	Credits += Delta;
	OnRep_CredistChanged(Delta);
	
}

bool ASPlayerState::RemoveCredits(const int32 Delta)
{
	// Avoid user error of substracting a negative amount or zero
	if(!ensure(Delta > 0))
	{
		return false;
	}

	if(Credits < Delta)
	{
		// not enough credits available
		return false;
	}

	Credits -= Delta;

	OnRep_CredistChanged(Delta);

	return true;
}

void ASPlayerState::OnRep_CredistChanged(int32 Delta)
{
	OnCreditsChanged.Broadcast(this, Credits, Delta);
}




int32 ASPlayerState::GetCredits() const
{
	return Credits;
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Credits);
	DOREPLIFETIME(ThisClass, OnCreditsChanged);
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if(SaveObject)
	{
		SaveObject->GetPlayerData(this)->Credits = Credits;
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if(SaveObject)
	{
		//Credits = SaveObject->Credits;
		// for trigger multiplayer
		AddCredits(SaveObject->GetPlayerData(this)->Credits);
	}
}