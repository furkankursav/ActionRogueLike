// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (ensure(BlackboardComp))
	{
		const AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(FName("TargetActor")));

		if (TargetActor)
		{
			const AAIController* MyController = OwnerComp.GetAIOwner();

			if (ensure(MyController))
			{
				APawn* AIPawn = MyController->GetPawn();

				if (ensure(AIPawn))
				{
					const float Distance = FVector::Distance(TargetActor->GetActorLocation(),
					                                         AIPawn->GetActorLocation());

					const bool bWithinRange = Distance < 2000.f;
					bool bHasLOS = false;
					if (bWithinRange)
					{
						bHasLOS = MyController->LineOfSightTo(TargetActor);
					}
					BlackboardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, bWithinRange && bHasLOS);
				}
			}
		}
	}
	
}
