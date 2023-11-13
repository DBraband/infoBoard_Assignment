// Copyright Epic Games, Inc. All Rights Reserved.

#include "HideAndSeekGameMode.h"
#include "HideAndSeekPlayerController.h"
#include "HideAndSeekCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHideAndSeekGameMode::AHideAndSeekGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AHideAndSeekPlayerController::StaticClass();
	DefaultPawnClass = AHideAndSeekCharacter::StaticClass();
	PlayerControllerClass = AHideAndSeekPlayerController::StaticClass();

}