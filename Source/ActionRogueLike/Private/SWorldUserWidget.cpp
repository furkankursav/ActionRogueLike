// Fill out your copyright notice in the Description page of Project Settings.


#include "SWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"

void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(IsValid(AttachedActor) == false)
	{
		RemoveFromParent();
		UE_LOG(LogTemp, Warning, TEXT("AttachedActor no longer valid, removing Health Widget"));
		return;
	}
	
	FVector2D ScreenPosition;
	const bool bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, ScreenPosition);
	if (bIsOnScreen)
	{
		const float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
		ScreenPosition /= ViewportScale;

		if(ParentSizeBox)
		{
			ParentSizeBox->SetRenderTranslation(ScreenPosition);
		}
	}
	if(ParentSizeBox)
	{
		const ESlateVisibility WidgetVisibility = (bIsOnScreen) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;
		ParentSizeBox->SetVisibility(WidgetVisibility);
	}
	
}
