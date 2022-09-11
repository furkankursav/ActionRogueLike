// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SBasePowerup.h"
#include "SCharacter.h"
#include "SMonsterData.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "SSaveGameSubsystem.h"
#include "ActionRogueLike/ActionRogueLike.h"
#include "ActionSystem/SActionComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "AI/SAICharacter.h"
#include "Engine/AssetManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via table"), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	CreditsPerKill = 20;

	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 2000;

	PlayerStateClass = ASPlayerState::StaticClass();

}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
	
	const FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	
	if(SG)
	{
		SG->LoadSaveGame(SelectedSaveSlot);
	}
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	// Continuos timer to spawn in more bots.
	// Actual amount of bots and whter its allowed to spawn determined by spawn logic later in chain...
	GetWorldTimerManager().SetTimer(SpawnBots_TimerHandle, this, &ASGameModeBase::SpawnBotsTimerElapsed, SpawnTimerInterval, true);

	// Make sure we have assigned at least one power-up class
	if(ensure(PowerupClasses.Num() > 0))
	{
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PowerupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
		if(ensure(QueryInstance))
		{
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPowerupSpawnQueryCompleted);
		}
	}
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Calling Before Super:: so we set variables before 'beginplayingstate' is called in PlayerController (which is where we instantiate UI)
	USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
	SG->HandleStartingNewPlayer(NewPlayer);

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// Now we're ready to override spawn location
	// Alternatively we could override core spawn location to use store locations immediately (skipping the whole 'find player start' logic)
	SG->OverrideSpawnTransform(NewPlayer);
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

	if(CVarSpawnBots.GetValueOnGameThread() == false || bCanSpawnBots == false)
	{
		//UE_LOG(LogTemp, Warning, TEXT("bot spawning disabled via cvar 'CVarSpawnBots' ."))
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
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnBotSpawnQueryCompleted);
	}
}

void ASGameModeBase::OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
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

		if(MonsterTable)
		{
			TArray<FMonsterInfoRow*> Rows;
			MonsterTable->GetAllRows("", Rows);

			if(Rows.Num() > 0)
			{
				const int32 RandRowIndex = FMath::RandRange(0, Rows.Num() - 1);

				if(const FMonsterInfoRow* SelectedRow = Rows[RandRowIndex])
				{
					if(UAssetManager* MyAssetManager = UAssetManager::GetIfValid())
					{
						LogOnScreen(this, "Monster Loading...", FColor::Green);
						const TArray<FName> Bundles;
						const FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnMonsterLoaded, SelectedRow->MonsterDataID, FoundLocations[0]);
						MyAssetManager->LoadPrimaryAsset(SelectedRow->MonsterDataID, Bundles, Delegate);
					}
					
					
				}
			}
		}
		
		
	}

}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	if(const UAssetManager* MyAssetManager = UAssetManager::GetIfValid())
	{
		if(USMonsterData* MonsterData = Cast<USMonsterData>(MyAssetManager->GetPrimaryAssetObject(LoadedId)))
		{
			LogOnScreen(this, "Monster Loaded!", FColor::Red);

			if(AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator))
			{
				LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

				if(USActionComponent* ActionComp = NewBot->FindComponentByClass<USActionComponent>())
				{
					for(const TSubclassOf<USAction> ActionClass : MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}
			}
		}
	}
	
	
}

void ASGameModeBase::OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
                                                  EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn power-up EQS Query failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	// Keep used locations to easy check distance between points
	TArray<FVector> UsedLocations;

	int32 SpawnCounter = 0;

	// Break out if we reached the desired count or if we have no more potential positions remaining
	while(SpawnCounter < DesiredPowerupCount && Locations.Num() > 0)
	{
		// Pick a random location from remaining points
		const int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);

		FVector PickedLocation = Locations[RandomLocationIndex];
		// Remove to avoid picking again
		Locations.RemoveAt(RandomLocationIndex);

		// Check minimum distance requirement
		bool bValidLocation = true;
		for(FVector OtherLocation : UsedLocations)
		{
			const float DistanceTo = (PickedLocation - OtherLocation).Size();

			if(DistanceTo < RequiredPowerupDistance)
			{
				// Show skipped location due to distance
				//DrawDebugSphere(GetWorld(), PickedLocation, 50.0f, 20, FColor::Red, false, 10.0f);

				// too close, skip to next attempt
				bValidLocation = false;
				break;
			}
		}

		// Failed the distance test
		if(bValidLocation == false)
		{
			continue;
		}

		// Pick a random powerup class
		const int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
		TSubclassOf<ASBasePowerup> RandomPowerupClass = PowerupClasses[RandomClassIndex];

		GetWorld()->SpawnActor<ASBasePowerup>(RandomPowerupClass, PickedLocation, FRotator::ZeroRotator);

		// Keep for distance checks
		SpawnCounter++;
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
	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));

	// Respawn players after delay.
	ASCharacter* Player = Cast<ASCharacter>(VictimActor);
	if(Player)
	{
		FTimerHandle TimerHandle_RespawnDelay;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, FName("RespawnPlayerElapsed"), Player->GetController());
		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}

	// Give Credits for kill
	APawn* KillerPawn = Cast<APawn>(Killer);
	if(KillerPawn)
	{
		ASPlayerState* PS = KillerPawn->GetPlayerState<ASPlayerState>();
		if(PS)
		{
			PS->AddCredits(CreditsPerKill);
		}
	}
}


