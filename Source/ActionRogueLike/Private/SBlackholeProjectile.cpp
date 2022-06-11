// Fill out your copyright notice in the Description page of Project Settings.


#include "SBlackholeProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ASBlackholeProjectile::ASBlackholeProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->RemoveObjectTypeToAffect(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	RadialForceComp->SetupAttachment(GetRootComponent());
	
	ProjectileMovementComp->ProjectileGravityScale = 0.f;
}

void ASBlackholeProjectile::SphereComp_OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	Super::SphereComp_OnComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,
	                                          SweepResult);

	if(OtherComp->IsSimulatingPhysics())
	{
		OtherActor->Destroy();
	}
}

// Called when the game starts or when spawned
void ASBlackholeProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASBlackholeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(RadialForceComp)
	{
		RadialForceComp->FireImpulse();
	}
}

