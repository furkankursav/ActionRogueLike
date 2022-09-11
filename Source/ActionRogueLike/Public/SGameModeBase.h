// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class ASAICharacter;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class USMonsterData;

/* Datatable Row for spawning monsters in game mode*/
USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	FMonsterInfoRow()
	{
		Weight = 1.0f;
		SpawnCost = 5.f;
		KillReward = 20.f;
	}

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FPrimaryAssetId MonsterDataID;

	/* Relative chance to pick this monster */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	/* Points required by game mode to spawn this unit.  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	/* Amount of credits awarded to killer of this unit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
};

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


protected:

	// UPROPERTY(EditDefaultsOnly, Category = "AI")
	// TSubclassOf<ASAICharacter> MinionRangedClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	FTimerHandle SpawnBots_TimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float SpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	int32 CreditsPerKill;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	UEnvQuery* PowerupSpawnQuery;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	TArray<TSubclassOf<class ASBasePowerup>> PowerupClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float RequiredPowerupDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 DesiredPowerupCount;

	UFUNCTION()
	void SpawnBotsTimerElapsed();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawner")
	bool bCanSpawnBots;

	UFUNCTION()
	void OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);


	void OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation);

	UFUNCTION()
	void OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);
	
public:

	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);
	
	ASGameModeBase();
	
	virtual void StartPlay() override;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UFUNCTION(Exec)
	void KillAll();



	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UDataTable* MonsterTable;



	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
};


