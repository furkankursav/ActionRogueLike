// Fill out your copyright notice in the Description page of Project Settings.


#include "SBasePowerup.h"
#include "Components/SphereComponent.h"

// Sets default values
ASBasePowerup::ASBasePowerup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName("Powerup");
	SetRootComponent(SphereComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(GetRootComponent());

	RespawnTime = 10.f;

}


void ASBasePowerup::ShowPowerup()
{
	SetPowerupState(true);
}

void ASBasePowerup::HideAndCoolDownPowerup()
{
	SetPowerupState(false);
	
	GetWorldTimerManager().SetTimer(Respawn_TimerHandle, this, &ASBasePowerup::ShowPowerup, RespawnTime);
}

void ASBasePowerup::SetPowerupState(bool bNewIsActive)
{
	SetActorEnableCollision(bNewIsActive);
	
	RootComponent->SetVisibility(bNewIsActive, true);
	MeshComp->SetVisibility(bNewIsActive, true);
}

void ASBasePowerup::Interact_Implementation(APawn* InstigatorPawn)
{
	// logic in derived classes..
}

