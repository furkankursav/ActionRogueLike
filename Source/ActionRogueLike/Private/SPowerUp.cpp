// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUp.h"

// Sets default values
ASPowerUp::ASPowerUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetRootComponent(MeshComp);


	ReactiveDelay = 5.f;

}

void ASPowerUp::OnPowerUpBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Cast<APawn>(OtherActor))
	{
		Execute_Interact(this, Cast<APawn>(OtherActor));
	}
}

// Called when the game starts or when spawned
void ASPowerUp::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->OnComponentBeginOverlap.AddDynamic(this, &ASPowerUp::OnPowerUpBeginOverlap);
	
}

// Called every frame
void ASPowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASPowerUp::Interact_Implementation(APawn* InstigatorPawn)
{
	ISGameplayInterface::Interact_Implementation(InstigatorPawn);
	SetActorHiddenInGame(true);

	FTimerHandle Reactive_TimerHandle;
	GetWorldTimerManager().SetTimer(Reactive_TimerHandle, [&]
	{
		SetActorHiddenInGame(false);
	}, ReactiveDelay, false);
	
}

