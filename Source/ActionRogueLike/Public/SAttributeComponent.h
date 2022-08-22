// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, AActor*, InstigatorActor, class USAttributeComponent*, OwningComp, float, NewValue, float, Delta);



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAttributeComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
	static USAttributeComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes", meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor* Actor);

protected:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float Rage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float MaxRage;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnRageChanged;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetHealth() const { return Health;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetHealthRatio() const { return Health / MaxHealth;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool IsFullHealth() const { return Health == MaxHealth;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetRage() const { return Rage; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetMaxRage() const { return MaxRage; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetRageRatio() const { return Rage / MaxRage;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool IsFullRage() const { return Rage == MaxRage;}
	

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	bool ApplyRageChange(AActor* InstigatorActor, float Delta);


	
	
	UFUNCTION(NetMulticast, Reliable) //@FIXME: mark as unrealiable once we moved the 'state' our of character
	void MulticastHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRageChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewRage, float Delta);

	
};
