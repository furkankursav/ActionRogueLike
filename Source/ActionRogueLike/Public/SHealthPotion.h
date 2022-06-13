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



public:

	ASHealthPotion();
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	
};
