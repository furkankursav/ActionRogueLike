// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRogueLike/Public/SMagicProjectile.h"

#include "SAttributeComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	SphereComp->SetCollisionProfileName("Projectile");
	FlightSound = CreateDefaultSubobject<UAudioComponent>(TEXT("FlightSound"));
	FlightSound->SetupAttachment(GetRootComponent());
	
	
	ProjectileMovementComp->InitialSpeed = 1000.f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bInitialVelocityInLocalSpace = true;

}

// Called when the game starts or when spawned
void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
	
}

void ASMagicProjectile::SphereComp_OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), GetActorRotation());
	}

	UGameplayStatics::PlayWorldCameraShake(this, CameraShakeClass, GetActorLocation(), 100, 200);

	Super::SphereComp_OnComponentHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

void ASMagicProjectile::SphereComp_OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::SphereComp_OnComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,
	                                          SweepResult);

	if(OtherActor && OtherActor != GetInstigator() && USAttributeComponent::GetAttributes(OtherActor))
	{

		if(ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), GetActorRotation());
		}
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(OtherActor);
		
		if(AttributeComp)
			AttributeComp->ApplyHealthChange(GetInstigator(), -DamageAmount);

		Destroy();
	}
}

// Called every frame
void ASMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

