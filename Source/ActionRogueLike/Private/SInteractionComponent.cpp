// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRogueLike/Public/SInteractionComponent.h"
#include "ActionRogueLike/Public/SGameplayInterface.h"


static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, TEXT("Enable Debug Lines for Interaction Component"), ECVF_Cheat);

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USInteractionComponent::PrimaryInteract()
{

	const bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();
	
	//FHitResult Hit;
	AActor* MyOwner = GetOwner();
	float Radius = 60.f;
	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector End = EyeLocation + (EyeRotation.Vector() * 1000.f);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	//bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

	TArray<FHitResult> Hits;
	FCollisionShape Shape;
	Shape.SetSphere(Radius);
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	FColor DebugLineColor = bBlockingHit ? FColor::Green : FColor::Red;
	for(FHitResult Hit : Hits)
	{
		if(bDebugDraw)
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, DebugLineColor, false, 2.f);
		
		if(AActor* HitActor = Hit.GetActor())
		{
			if(HitActor->Implements<USGameplayInterface>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);
				ISGameplayInterface::Execute_Interact(HitActor, MyPawn);
				
				break;
			}
		}
		
	}

	

	if(bDebugDraw)
		DrawDebugLine(GetWorld(), EyeLocation, End, DebugLineColor, false, 2.f, 0, 5);

	
}