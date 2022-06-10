// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRogueLike/Public/SMagicProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComponent->SetCollisionProfileName("Projectile");
	SetRootComponent(SphereComponent);

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EffectComp"));
	EffectComp->SetupAttachment(GetRootComponent());

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->InitialSpeed = 1000.f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

}

// Called when the game starts or when spawned
void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

