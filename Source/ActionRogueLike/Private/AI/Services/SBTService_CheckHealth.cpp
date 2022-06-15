// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/SBTService_CheckHealth.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const AAIController* AIController = OwnerComp.GetAIOwner();
	if(AIController == nullptr) return;
	
	APawn* AIPawn = AIController->GetPawn();
	if(AIPawn == nullptr) return;

	const USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(AIPawn);
	if(AttributeComp == nullptr) return;

	const bool bLowHealth = AttributeComp->IsAlive() && AttributeComp->GetHealth() < 30;
	
	OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool(IsLowHealthKey.SelectedKeyName, bLowHealth);
}
