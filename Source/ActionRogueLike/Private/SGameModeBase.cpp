// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "AI/SAICharacter.h"


ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(SpawnBots_TimerHandle, this, &ASGameModeBase::SpawnBotsTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::SpawnBotsTimerElapsed()
{
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if(ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);
	}
}

void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
									  EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Bot EQS Query Failed!"));
		return;
	}


	int32 NrOfAliveBots = 0;

	for(TActorIterator<ASAICharacter>It(GetWorld()); It; ++It)
	{
		const ASAICharacter* Bot = *It;
		
		const USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(Bot->GetComponentByClass(USAttributeComponent::StaticClass()));
		
		if(AttributeComp && AttributeComp->IsAlive())
		{
			NrOfAliveBots++;
		}
		
	}


	 float MaxBotCount = 10;

	

	if(DifficultyCurve)
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if(NrOfAliveBots >= MaxBotCount) return;
	
	TArray<FVector> FoundLocations = QueryInstance->GetResultsAsLocations();

	if(FoundLocations.Num() > 0 && FoundLocations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionRangedClass, FoundLocations[0], FRotator::ZeroRotator);
	}

}