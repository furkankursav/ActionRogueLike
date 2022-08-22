// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBasePowerup.h"
#include "ActionSystem/Actions/SAction.h"
#include "GameFramework/Actor.h"
#include "SPowerup_Action.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASPowerup_Action : public ASBasePowerup
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASPowerup_Action();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	TSubclassOf<USAction> ActionToGrant;
	
public:


	virtual void Interact_Implementation(APawn* InstigatorPawn) override;


};
