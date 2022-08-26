// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBasePowerup.h"
#include "SHealthPotion.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASHealthPotion : public ASBasePowerup
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Up")
	float HealthAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Up")
	int32 CreditCost;



public:

	ASHealthPotion();
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual FText GetInteractMessage_Implementation(APawn* InstigatorPawn) override;
	
};
