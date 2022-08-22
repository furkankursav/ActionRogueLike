// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FActorSaveData
{
	GENERATED_BODY()
	
public:
	// Identifier for which Actor this belongs to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
	FString ActorName;

	// For movable Actors, keep location, rotation and scale.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
	FTransform ActorTransform;

	UPROPERTY()
	TArray<uint8> ByteData;
};

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	int32 Credits;

	UPROPERTY()
	TArray<FActorSaveData> SavedActors;
};
