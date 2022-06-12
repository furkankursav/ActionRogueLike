// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/SBTTask_RangedAttack.h"

#include "AIController.h"
#include "SBaseProjectile.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* MyController = OwnerComp.GetAIOwner();

	if(ensure(MyController))
	{
		const ACharacter* MyCharacter = Cast<ACharacter>(MyController->GetPawn());

		if(MyCharacter == nullptr) return EBTNodeResult::Failed;

		const FVector MuzzleLocation = MyCharacter->GetMesh()->GetSocketLocation(FName("Muzzle_01"));

		const AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FName("TargetActor")));

		if(TargetActor == nullptr) return EBTNodeResult::Failed;

		const FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
		const FRotator MuzzleRotation = Direction.Rotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		const AActor* NewProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);

		return NewProjectile ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
