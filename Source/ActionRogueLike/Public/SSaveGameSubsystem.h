// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SSaveGameSubsystem.generated.h"

class USSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameSignature, class USSaveGame*, SaveObject);

/**
 * Handless al saving/loading of game state and player data including transform
 */
UCLASS(meta = (DisplayName = "SaveGame System")) // DisplayName creates prettier Blueprint Node
class ACTIONROGUELIKE_API USSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:

	/* Name of slot to save/load to disk. Filled by SaveGameSettings (can be overriden from GameMode's InitGame() */
	FString CurrentSlotName;

	UPROPERTY()
	USSaveGame* CurrentSaveGame;

public:

	/* Restore serialized data from PlayerState into player */
	void HandleStartingNewPlayer(AController* NewPlayer);

	/* Restore spawn transform using stored data per PlayerState after being fully initialized. */
	UFUNCTION(BlueprintCallable)
	bool OverrideSpawnTransform(AController* NewPlayer);

	/* Change slot name, will be used next time we save/load data. */
	UFUNCTION(BlueprintCallable)
	void SetSlotName(FString NewSlotName);

	UFUNCTION(BlueprintCallable, Category = "Save Game")
	void WriteSaveGame();

	/* */
	void LoadSaveGame(FString InSlotName = "");

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameLoaded;

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameWritten;

	/* Initialize subsystem, good moment to load in SaveGameSettings variables. */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
};
