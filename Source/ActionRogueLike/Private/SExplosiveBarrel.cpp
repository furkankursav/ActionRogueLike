// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRogueLike/Public/SExplosiveBarrel.h"

#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetSimulatePhysics(true);
	StaticMeshComp->SetCollisionProfileName("PhysicsActor");
	SetRootComponent(StaticMeshComp);

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->Radius = 697.f;
	RadialForceComp->ImpulseStrength = 2000.f;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	StaticMeshComp->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnStaticMeshCompHit);
	
}

void ASExplosiveBarrel::OnStaticMeshCompHit(UPrimitiveComponent* HitComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(RadialForceComp)
	{
		RadialForceComp->FireImpulse();
	}
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

