// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRogueLike/Public/SCharacter.h"

#include "SBlackholeProjectile.h"
#include "ActionRogueLike/Public/SInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ActionRogueLike/Public/SMagicProjectile.h"
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
	
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
	SpawnProjectile(MagicProjectileClass, MagicProjectileAttackMontage);
}

void ASCharacter::SecondaryAttack()
{
	SpawnProjectile(BlackholeProjectileClass, BlackholeProjectileAttackMontage);
}

void ASCharacter::TertiaryAttack()
{
	SpawnProjectile(DashProjectileClass, DashProjectileAttackMontage);
}

void ASCharacter::PrimaryInteract()
{
	if(InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::AttackDelay_Elapsed(TSubclassOf<ASProjectile> ProjectileClass, UAnimMontage* AttackMontage)
{
	if (ProjectileClass)
	{
		// Projectile spawn location
		const FVector SpawnLoc = GetMesh()->GetSocketLocation(FName("Muzzle_01"));

		// Selecting target location
		FHitResult CameraHitResult;
		// Start point is camera's world location
		FVector CameraLoc = CameraComp->GetComponentLocation();
		// End point is where camera is looking * 10000 times forward
		FVector CameraEnd = CameraLoc + (CameraComp->GetComponentRotation().Vector() * 10000);

		// Trace only will react to world dynamic and world static object types.
		FCollisionObjectQueryParams CameraParams;
		CameraParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		CameraParams.AddObjectTypesToQuery(ECC_WorldStatic);

		// firing trace
		bool bCameraHit = GetWorld()->LineTraceSingleByObjectType(CameraHitResult, CameraLoc, CameraEnd, CameraParams);

		// if trace was hit chose impact point else, chose trace end location.
		FVector DesiredTargetLoc = bCameraHit ? CameraHitResult.ImpactPoint : CameraHitResult.TraceEnd;

		// calculating direction from spawn loc to target location
		FRotator SpawnRot = UKismetMathLibrary::FindLookAtRotation(SpawnLoc, DesiredTargetLoc);

		// debugging trace
		FColor DebugColor = bCameraHit ? FColor::Green : FColor::Red;
		DrawDebugLine(GetWorld(), CameraLoc, DesiredTargetLoc, DebugColor, false, 2.f, 0, 2.f);

		// spawning projectile
		FTransform SpawnTM = FTransform(SpawnRot, SpawnLoc);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
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

void ASCharacter::SpawnProjectile(TSubclassOf<ASProjectile> ProjectileClass, UAnimMontage* AttackMontage, float WaitTime)
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

