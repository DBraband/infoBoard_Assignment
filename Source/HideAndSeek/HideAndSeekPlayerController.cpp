// Copyright Epic Games, Inc. All Rights Reserved.

#include "HideAndSeekPlayerController.h"

#include <string>

#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "HideAndSeekCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"

AHideAndSeekPlayerController::AHideAndSeekPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AHideAndSeekPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void AHideAndSeekPlayerController::SetPlayerControllerActivated(bool state)
{
	PlayerControllerActivated = state;
}

void AHideAndSeekPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this,
		                                   &AHideAndSeekPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this,
		                                   &AHideAndSeekPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(CameraLeftAction, ETriggerEvent::Triggered, this,
										   &AHideAndSeekPlayerController::OnCameraRotateLeft);
		EnhancedInputComponent->BindAction(CameraRightAction, ETriggerEvent::Triggered, this,
										   &AHideAndSeekPlayerController::OnCameraRotateRight);
	}
}

void AHideAndSeekPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void AHideAndSeekPlayerController::OnSetDestinationTriggered()
{
	if (PlayerControllerActivated)
	{
		// We flag that the input is being pressed
		FollowTime += GetWorld()->GetDeltaSeconds();

		// We look for the location in the world where the player has pressed the input
		FHitResult Hit;
		bool bHitSuccessful = false;

		bHitSuccessful = GetHitResultUnderCursor(ECC_Visibility, true, Hit);


		// If we hit a surface, cache the location
		if (bHitSuccessful)
		{
			CachedDestination = Hit.Location;
		}

		// Move towards mouse pointer or touch
		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn != nullptr)
		{
			FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
		}
	}
}

void AHideAndSeekPlayerController::OnCameraRotateRight()
{
	if (auto* player = Cast<AHideAndSeekCharacter>(GetCharacter()))
	{
		player->RotateCamera(1.0f);
	}
}

void AHideAndSeekPlayerController::OnCameraRotateLeft()
{
	if (auto* player = Cast<AHideAndSeekCharacter>(GetCharacter()))
	{
		player->RotateCamera(-1.0f);
	}
}
