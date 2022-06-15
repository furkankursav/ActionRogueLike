// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_RangedAttack.generated.h"


class ASBaseProjectile;

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USBTTask_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()


public:
	USBTTask_RangedAttack();
	
protected:


	UPROPERTY(EditAnywhere)
	float MaxBulletSpread;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<ASBaseProjectile> ProjectileClass;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
