// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRogueLike/Public/SCharacter.h"

#include "SAttributeComponent.h"
#include "SBlackholeProjectile.h"
#include "ActionRogueLike/Public/SInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ActionRogueLike/Public/SMagicProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(GetRootComponent());

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>(TEXT("InteractionComp"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));

	AttackAnimDelay = 0.2f;
	HandSocketName = FName("Muzzle_01");
	TimeToHitParamName = FName("TimeToHit");
	
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.f;
	ControlRot.Roll = 0.f;
	AddMovementInput(ControlRot.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.f;
	ControlRot.Roll = 0.f;

	const FVector ControllerRightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	
	AddMovementInput(ControllerRightVector, Value);
}

void ASCharacter::RotationVisualization()
{
	const float DrawScale = 100.f;
	const float Thickness = 5.0f;

	FVector LineStart = GetActorLocation();

	// Offset to the right of pawn
	LineStart += GetActorRightVector() * 100.f;
	FVector ActorDirection_LineEnd = LineStart + GetActorForwardVector() * 100.f;
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.f, 0, Thickness);

	FVector ControllerDirection_LineEnd = LineStart + GetControlRotation().Vector() * 100.f;
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.f, 0, Thickness);
}

void ASCharacter::PrimaryAttack()
{
	if(CastingParticle)
	{
		UGameplayStatics::SpawnEmitterAttached(CastingParticle, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
	}
	
	SpawnProjectile(MagicProjectileClass, MagicProjectileAttackMontage, AttackAnimDelay);
}

void ASCharacter::SecondaryAttack()
{
	SpawnProjectile(BlackholeProjectileClass, BlackholeProjectileAttackMontage, AttackAnimDelay);
}

void ASCharacter::TertiaryAttack()
{
	SpawnProjectile(DashProjectileClass, DashProjectileAttackMontage, AttackAnimDelay);
}

void ASCharacter::PrimaryInteract()
{
	if(InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::AttackDelay_Elapsed(TSubclassOf<ASBaseProjectile> ProjectileClass, UAnimMontage* AttackMontage)
{
	if (ProjectileClass)
	{
		// Projectile spawn parameters
		const FVector HandLocation = GetMesh()->GetSocketLocation(HandSocketName);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;

		FCollisionShape Shape;
		Shape.SetSphere(20.f);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);

		FVector TraceStart = CameraComp->GetComponentLocation();

		FVector TraceEnd = TraceStart + GetControlRotation().Vector() * 5000.f;

		FHitResult Hit;

		if(GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjParams, Shape, Params))
		{
			TraceEnd = Hit.ImpactPoint;
		}

		// find new direction/rotation from Hand Point to impact point in world.
		FRotator ProjRotation = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();

		// spawning projectile
		FTransform SpawnTM = FTransform(ProjRotation, HandLocation);
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	
	if(NewHealth <= 0.f && Delta < 0.0f)
	{
		DisableInput(Cast<APlayerController>(GetController()));
	}

	if(Delta < 0.f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//RotationVisualization();
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::SecondaryAttack);
	PlayerInputComponent->BindAction("TertiaryAttack", IE_Pressed, this, &ASCharacter::TertiaryAttack);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);

}

void ASCharacter::SpawnProjectile(TSubclassOf<ASBaseProjectile> ProjectileClass, UAnimMontage* AttackMontage, float WaitTime)
{
	if(ProjectileClass)
	{

		if (AttackMontage)
		{
			PlayAnimMontage(AttackMontage);
		}
		
		AttackDelay_Delegate.BindUFunction(this, "AttackDelay_Elapsed", ProjectileClass, AttackMontage);
		GetWorldTimerManager().SetTimer(AttackDelay_TimerHandle, AttackDelay_Delegate, WaitTime, false);
	}
}

