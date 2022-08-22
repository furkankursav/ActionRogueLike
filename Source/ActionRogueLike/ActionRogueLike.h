// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

static void LogOnScreen(UObject* WorldContextObject, FString Msg, FColor Color = FColor::White, float Duration = 5.f)
{
	if( !ensure(WorldContextObject))
	{
		return;
	}

	const UWorld* World = WorldContextObject->GetWorld();

	if(!ensure(World))
	{
		return;
	}

	const FString NetPrefix = World->IsNetMode(NM_Client) ? "[CLIENT] " : "[SERVER] ";

	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, NetPrefix + Msg);
	}
}