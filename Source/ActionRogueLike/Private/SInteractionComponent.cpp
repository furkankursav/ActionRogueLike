// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRogueLike/Public/SInteractionComponent.h"
#include "ActionRogueLike/Public/SGameplayInterface.h"
#include "Blueprint/UserWidget.h"
#include "ActionRogueLike/Public/SWorldUserWidget.h"


static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, TEXT("Enable Debug Lines for Interaction Component"), ECVF_Cheat);


USInteractionComponent::USInteractionComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	TraceDistance = 1000.f;
	TraceRadius = 60.f;

	CollisionChannel = ECC_WorldDynamic;
}





// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if(OwnerPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}
	
}

void USInteractionComponent::FindBestInteractable()
{
	const bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();
	
	//FHitResult Hit;
	AActor* MyOwner = GetOwner();
	
	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	TArray<FHitResult> Hits;
	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	FColor DebugLineColor = bBlockingHit ? FColor::Green : FColor::Red;

	// clear ref before try refill
	FocusedInteractable = nullptr;
	
	for(FHitResult Hit : Hits)
	{
		if(bDebugDraw)
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 32, DebugLineColor, false, 2.f);
		
		if(AActor* HitActor = Hit.GetActor())
		{
			if(HitActor->Implements<USGameplayInterface>())
			{
				FocusedInteractable = HitActor;
				break;
			}
		}
		
	}

	if(FocusedInteractable)
	{
		if(InteractionWidgetInstance == nullptr && InteractionWidgetClass != nullptr)
		{
			InteractionWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), InteractionWidgetClass);
		}

		if(InteractionWidgetInstance)
		{
			InteractionWidgetInstance->AttachedActor = FocusedInteractable;
			
			if(InteractionWidgetInstance->IsInViewport() == false)
				InteractionWidgetInstance->AddToViewport();
		}
	}

	else
	{
		if(InteractionWidgetInstance)
		{
			InteractionWidgetInstance->RemoveFromParent();
		}
	}
	
	
	if(bDebugDraw)
		DrawDebugLine(GetWorld(), EyeLocation, End, DebugLineColor, false, 2.f, 0, 5);
	
}

void USInteractionComponent::PrimaryInteract()
{
	ServerInteract(FocusedInteractable);
}

void USInteractionComponent::ServerInteract_Implementation(AActor* InFocus)
{
	if(InFocus == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No Focus Interactable to Interact!"));
		return;
	}
	
	APawn* MyPawn = Cast<APawn>(GetOwner());
	ISGameplayInterface::Execute_Interact(InFocus, MyPawn);
}