// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBaseProjectile.h"
#include "SDashProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASDashProjectile : public ASBaseProjectile
{
	GENERATED_BODY()
public:
	ASDashProjectile();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float TeleportDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float DetonateDelay;

	FTimerHandle DelayedDetonate_TimerHandle;

	virtual void Explode_Implementation() override;

	void TeleportInstigator();

	FTimerHandle Teleport_TimerHandle;
	FTimerDelegate Teleport_Delegate;

	FTimerHandle Explode_TimerHandle;

	virtual void BeginPlay() override;
	
};
