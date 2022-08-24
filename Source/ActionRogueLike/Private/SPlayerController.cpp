// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ASPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	
	BlueprintBeginPlayingState();
}

void ASPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	OnPlayerStateReceived.Broadcast(PlayerState);
}

void ASPlayerController::TogglePauseMenu()
{

	if(PauseMenuInstance && PauseMenuInstance->IsInViewport())
	{
		PauseMenuInstance->RemoveFromParent();
		PauseMenuInstance = nullptr;

		bShowMouseCursor = false;

		SetInputMode(FInputModeGameOnly());
		UGameplayStatics::SetGamePaused(this, false);
		return;
	}

	if(PauseMenuClass)
	{
		PauseMenuInstance = CreateWidget<UUserWidget>(GetWorld(), PauseMenuClass);

		if(PauseMenuInstance)
		{
			PauseMenuInstance->AddToViewport(100);

			bShowMouseCursor = true;
	
			SetInputMode(FInputModeUIOnly());
			UGameplayStatics::SetGamePaused(this, true);
		}
	}
	
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("PauseMenu", IE_Pressed, this, &ThisClass::TogglePauseMenu);
}
