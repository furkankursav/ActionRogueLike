// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, class ASPlayerState*, PlayerState, int32, NewCredits, int32, Delta);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, ReplicatedUsing="OnRep_CredistChanged", Category = "Credits")
	int32 Credits;

public:

	ASPlayerState();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Credits")
	int32 GetCredits() const;

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void AddCredits(const int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool RemoveCredits(const int32 Delta);

	UPROPERTY(BlueprintAssignable, Replicated, Category = "Events")
	FOnCreditsChanged OnCreditsChanged;

	UFUNCTION()
	void OnRep_CredistChanged(int32 Delta);

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(class USSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(class USSaveGame* SaveObject);

	
};
