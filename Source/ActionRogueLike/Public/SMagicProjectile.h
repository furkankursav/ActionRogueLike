// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SBaseProjectile.h"
#include "SMagicProjectile.generated.h"

class UAudioComponent;
class USoundBase;
class UCameraShakeBase;
class USActionEffect;


UCLASS()
class ACTIONROGUELIKE_API ASMagicProjectile : public ASBaseProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASMagicProjectile();

protected:


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float DamageAmount = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAudioComponent* FlightSound;


	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag ParryTag;

	UPROPERTY(EditDefaultsOnly, Category= "Effect")
	TSubclassOf<USActionEffect> BurningActionEffectClass;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SphereComp_OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void SphereComp_OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
