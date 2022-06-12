// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if(ensureAlwaysMsgf(BehaviorTree, TEXT("Behaviour tree is nullptr! Please assign BehaviourTree in your AI Controller")))
	{
		RunBehaviorTree(BehaviorTree);
	}

	// APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	//
	// if(PlayerPawn)
	// {
	// 	if(GetBlackboardComponent())
	// 	{
	// 		GetBlackboardComponent()->SetValueAsVector(FName("MoveToLocation"), PlayerPawn->GetActorLocation());
	// 		GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), PlayerPawn);
	// 	}
	// }
}
