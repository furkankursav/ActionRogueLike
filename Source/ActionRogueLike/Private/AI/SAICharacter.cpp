// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));

	HitFlashTimeName = "HitFlashTime";

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	AAIController* AIC =  Cast<AAIController>(GetController());

	if(AIC)
	{
		UBlackboardComponent* BBComp = AIC->GetBlackboardComponent();
		BBComp->SetValueAsObject(FName("TargetActor"), Pawn);

		DrawDebugString(GetWorld(), Pawn->GetActorLocation(), TEXT("PLAYER SPOTTED"), nullptr, FColor::Red, 4.f, true);
	}
}

void ASAICharacter::OnAIHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float Health,
	float Delta)
{

	if(Health <= 0.f)
	{
		Destroy();
	}
	
	if(Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(HitFlashTimeName, GetWorld()->TimeSeconds);
	}
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnAIHealthChanged);
}

