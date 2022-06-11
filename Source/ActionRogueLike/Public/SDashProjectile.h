// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBaseProjectile.h"
#include "SDashProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASDashProjectile : public ASProjectile
{
	GENERATED_BODY()

protected:

	FTimerHandle Teleport_TimerHandle;
	FTimerDelegate Teleport_Delegate;

	FTimerHandle Explode_TimerHandle;

	UPROPERTY(EditAnywhere, Category = "Teleport", meta=(AllowPrivateAccess = "true"))
	class UParticleSystem* TeleportEmitter;
	
	UFUNCTION()
	void Teleport_Elapsed(class APawn* InstigatorPawn);

	UFUNCTION()
	void Explode_Elapsed();

	virtual void BeginPlay() override;

	virtual void SphereComp_OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
