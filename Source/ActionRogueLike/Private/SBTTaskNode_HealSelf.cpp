// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTTaskNode_HealSelf.h"

#include "AIController.h"
#include "SAttributeComponent.h"

EBTNodeResult::Type USBTTaskNode_HealSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if(AIController == nullptr) return EBTNodeResult::Type::Failed;

	APawn* AIPawn = AIController->GetPawn();

	if(AIPawn == nullptr) return EBTNodeResult::Type::Failed;

	USAttributeComponent* AIAttributeComp = Cast<USAttributeComponent>(AIPawn->GetComponentByClass(USAttributeComponent::StaticClass()));

	if(AIAttributeComp == nullptr) return EBTNodeResult::Type::Failed;

	AIAttributeComp->ApplyHealthChange(50);

	return EBTNodeResult::Type::Succeeded;
}
