// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRogueLike/Public/ASItemChest.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AASItemChest::AASItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	SetRootComponent(BaseMesh);

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(GetRootComponent());

	TargetPitch = 110.f;

}

// Called when the game starts or when spawned
void AASItemChest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AASItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	ISGameplayInterface::Interact_Implementation(InstigatorPawn);

	LidMesh->SetRelativeRotation(FRotator(TargetPitch, 0.f, 0.f));
}

