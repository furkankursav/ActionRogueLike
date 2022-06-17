// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "AI/SAICharacter.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via table"), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(SpawnBots_TimerHandle, this, &ASGameModeBase::SpawnBotsTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::KillAll()
{
	for(TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* BotAttributeComp = USAttributeComponent::GetAttributes(Bot);
		if(ensure(BotAttributeComp) && BotAttributeComp->IsAlive())
		{
			BotAttributeComp->Kill(this); //@fixme pass in player for kill credit
		}
	}
}

void ASGameModeBase::SpawnBotsTimerElapsed()
{

	if(CVarSpawnBots.GetValueOnGameThread() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("bot spawning disabled via cvar 'CVarSpawnBots' ."))
		return;
	}

	int32 NrOfAliveBots = 0;

	for(TActorIterator<ASAICharacter>It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		
		const USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		
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

	if(NrOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("At maximum bot capacity. Skipping bot spawn!"));
		return;
	}
	
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


	TArray<FVector> FoundLocations = QueryInstance->GetResultsAsLocations();

	if(FoundLocations.Num() > 0 && FoundLocations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionRangedClass, FoundLocations[0], FRotator::ZeroRotator);
		DrawDebugSphere(GetWorld(), FoundLocations[0], 50, 20, FColor::Blue, false, 60.f);
	}

}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if(ensure(Controller))
	{
		Controller->UnPossess();
		RestartPlayer(Controller);
	}
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	ASCharacter* Player = Cast<ASCharacter>(VictimActor);

	if(Player)
	{
		FTimerHandle TimerHandle_RespawnDelay;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, FName("RespawnPlayerElapsed"), Player->GetController());
		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}
}
