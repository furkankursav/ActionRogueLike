// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SBasePowerup.generated.h"

class USphereComponent;

UCLASS(Abstract)
class ACTIONROGUELIKE_API ASBasePowerup : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASBasePowerup();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Powerup")
	float RespawnTime;

	UPROPERTY(ReplicatedUsing="OnRep_IsActive")
	bool bIsActive;

	UFUNCTION()
	void OnRep_IsActive();

	FTimerHandle Respawn_TimerHandle;

	UFUNCTION()
	void ShowPowerup();

	UFUNCTION()
	void HideAndCoolDownPowerup();

	UFUNCTION()
	void SetPowerupState(bool bNewIsActive);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

public:	

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual FText GetInteractMessage_Implementation(APawn* InstigatorPawn) override;

};
