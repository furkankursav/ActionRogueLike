// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBasePowerup.h"
#include "SPowerup_Credits.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPowerup_Credits : public ASBasePowerup
{
	GENERATED_BODY()
public:

	ASPowerup_Credits();
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Powerup")
	int32 CreditsAmount;
	
};
