// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SBasePowerup.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "AI/SAICharacter.h"
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

	SaveSlotName = "SaveGame01";
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	LoadSaveGame();
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
	

	ASPlayerState* PS = Cast<ASPlayerState>(NewPlayer->PlayerState);

	if(PS)
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
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
		GetWorld()->SpawnActor<AActor>(MinionRangedClass, FoundLocations[0], FRotator::ZeroRotator);
		DrawDebugSphere(GetWorld(), FoundLocations[0], 50, 20, FColor::Blue, false, 60.f);
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

void ASGameModeBase::WriteSaveGame()
{
	for (TObjectPtr<APlayerState> UnrealPlayerState : GameState->PlayerArray)
	{
		if(ASPlayerState* PS = Cast<ASPlayerState>(UnrealPlayerState))
		{
			PS->SavePlayerState(CurrentSaveGame);
			break; // single player only at this point
		}
	}

	CurrentSaveGame->SavedActors.Empty();

	for(FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* NextActor = *It;
		if(NextActor->IsPendingKillPending() || NextActor->Implements<USGameplayInterface>() == false)
		{
			continue;
		}
		FActorSaveData ActorSaveData;
		ActorSaveData.ActorName = NextActor->GetName();
		ActorSaveData.ActorTransform = NextActor->GetTransform();

		// Pass the array to fill with data from Actor
		FMemoryWriter MemWriter(ActorSaveData.ByteData);
		
		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		
		// Find only variables with UPROPERTY(SaveGame)
		Ar.ArIsSaveGame = true;

		// Coverts Actor's SaveGame UPROPERTIES into binary array.
		NextActor->Serialize(Ar);
		
		CurrentSaveGame->SavedActors.Add(ActorSaveData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SaveSlotName, 0);
	
}

void ASGameModeBase::LoadSaveGame()
{
	if(UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));

		if(CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame data."));
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Loaded SaveGame data."));

		for(FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* NextActor = *It;
			if(NextActor->IsPendingKillPending() || NextActor->Implements<USGameplayInterface>() == false)
			{
				continue;
			}

			for(FActorSaveData NextData : CurrentSaveGame->SavedActors)
			{
				if(NextData.ActorName == NextActor->GetName())
				{
					NextActor->SetActorTransform(NextData.ActorTransform);

					FMemoryReader MemReader(NextData.ByteData);

					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					Ar.ArIsSaveGame = true;
					// Covert binary array back into Actor's variables.
					NextActor->Serialize(Ar);

					ISGameplayInterface::Execute_OnSaveableActorLoaded(NextActor);
					break;
				}
			}
		}
	}

	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
		UE_LOG(LogTemp, Warning, TEXT("Created New SaveGame data."));
	}


}


