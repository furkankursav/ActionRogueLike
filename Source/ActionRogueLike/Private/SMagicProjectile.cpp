// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRogueLike/Public/SMagicProjectile.h"

#include "SGameplayFunctionLibrary.h"
#include "ActionSystem/SActionComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "ActionRogueLike/Public/ActionSystem/ActionEffect/SActionEffect.h"

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
	

	Super::SphereComp_OnComponentHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

void ASMagicProjectile::SphereComp_OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if(OtherActor && OtherActor != GetInstigator())
	{
		
		
		USActionComponent* ActionComp = OtherActor->FindComponentByClass<USActionComponent>();
		if(ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			ProjectileMovementComp->Velocity *= -1;
			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}

		// Apply damage & Impulse
		if (USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			if(ActionComp)
			{
				ActionComp->AddAction(GetInstigator(), BurningActionEffectClass);
			}
			
			Explode();
			
		}
	}
}

// Called every frame
void ASMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

