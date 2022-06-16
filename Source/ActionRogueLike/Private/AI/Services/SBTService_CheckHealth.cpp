// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/SBTService_CheckHealth.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


USBTService_CheckHealth::USBTService_CheckHealth()
{
	LowHealthFraction = 0.3f;
}

void USBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	
	APawn* AIPawn =  OwnerComp.GetAIOwner()->GetPawn();

	if(ensure(AIPawn))
	{
		const USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(AIPawn);
		if(ensure(AttributeComp))
		{
			const bool bLowHealth = (AttributeComp->GetHealth() / AttributeComp->GetMaxHealth()) < LowHealthFraction;
			OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool(IsLowHealthKey.SelectedKeyName, bLowHealth);
		}
	}
	
	
}


