// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class USInteractionComponent;
class UAnimMontage;
class ASBaseProjectile;
class USAttributeComponent;

UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<ASBaseProjectile> MagicProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<ASBaseProjectile> BlackholeProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<ASBaseProjectile> DashProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* MagicProjectileAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* BlackholeProjectileAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* DashProjectileAttackMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	USInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComp;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* CastingParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	FName HandSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	FName TimeToHitParamName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	float AttackAnimDelay;
	
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);


	void RotationVisualization();

	void PrimaryAttack();

	void SecondaryAttack();

	void TertiaryAttack();

	void PrimaryInteract();

	FTimerHandle AttackDelay_TimerHandle;
	FTimerDelegate AttackDelay_Delegate;

	UFUNCTION()
	void AttackDelay_Elapsed(TSubclassOf<ASBaseProjectile> ProjectileClass, UAnimMontage* AttackMontage);

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(TSubclassOf<ASBaseProjectile> ProjectileClass, UAnimMontage* AttackMontage, float WaitTime);

	UFUNCTION(Exec)
	void HealSelf(float Amount = 100);

};
