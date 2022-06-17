// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class ASAICharacter;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


protected:

	FTimerHandle SpawnBots_TimerHandle;

	UFUNCTION()
	void SpawnBotsTimerElapsed();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float SpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<ASAICharacter> MinionRangedClass;

	UFUNCTION()
	void OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);
	
public:

	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);
	
	ASGameModeBase();
	
	virtual void StartPlay() override;

	UFUNCTION(Exec)
	void KillAll();
};
