// Fill out your copyright notice in the Description page of Project Settings.


#include "SDashProjectile.h"

#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void ASDashProjectile::Teleport_Elapsed(APawn* InstigatorPawn)
{
	if(InstigatorPawn)
	{
		ProjectileMovementComp->StopMovementImmediately();
		InstigatorPawn->GetMovementComponent()->StopMovementImmediately();
		InstigatorPawn->TeleportTo(GetActorLocation(), InstigatorPawn->GetActorRotation(),false);
		Destroy();
	}
}

void ASDashProjectile::Explode_Elapsed()
{
	ProjectileMovementComp->StopMovementImmediately();
	if(TeleportEmitter) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TeleportEmitter, GetActorLocation(), GetActorRotation());
	GetWorldTimerManager().ClearAllTimersForObject(this);
	GetWorldTimerManager().SetTimer(Teleport_TimerHandle, Teleport_Delegate, 0.2f, false);
}

void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	Teleport_Delegate.BindUFunction(this, "Teleport_Elapsed", GetInstigator());
	GetWorldTimerManager().SetTimer(Explode_TimerHandle, this, &ASDashProjectile::Explode_Elapsed, 0.2f, false);
}

void ASDashProjectile::SphereComp_OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::SphereComp_OnComponentHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	GetWorldTimerManager().ClearAllTimersForObject(this);
	Explode_Elapsed();
	
	
}
