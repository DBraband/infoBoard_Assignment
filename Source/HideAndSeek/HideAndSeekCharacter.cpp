// Copyright Epic Games, Inc. All Rights Reserved.

#include "HideAndSeekCharacter.h"

#include <string>

#include "HideAndSeekHUD.h"
#include "HideAndSeekPlayerController.h"
#include "NPC_Character.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Sight.h"

AHideAndSeekCharacter::AHideAndSeekCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 84.f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	AutoReceiveInput = EAutoReceiveInput::Player0;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 840.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->MaxWalkSpeed = 700.0f;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	// Set Arm Length to balance out the low FOV of the camera
	CameraBoom->TargetArmLength = 7000.f;
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	CameraBoom->bUsePawnControlRotation = false;

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->SetFieldOfView(MainMenuCameraFOV);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Player cant control rotation

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Setup Box Collider, to determine if Player got found
	// Setup Bush Box Collider
	PlayerBoxCollider = CreateDefaultSubobject<UBoxComponent>(FName("TriggerVolume"));
	PlayerBoxCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AHideAndSeekCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (hidingPhaseActive && GetWorld()->GetTimerManager().IsTimerActive(HidingPhaseTimerHandle))
	{		
		if (HudReferenceHiding != nullptr)
		{
			HudReferenceHiding->SetCountdownText(GetWorld()->GetTimerManager().GetTimerRemaining(HidingPhaseTimerHandle),
				UserHUDComponentHidingInstance, hidingPhaseTime);
		}
	}
	else if (searchingPhaseActive && GetWorld()->GetTimerManager().IsTimerActive(SearchingPhaseTimerHandle))
	{
		if (HudReferenceSearching != nullptr)
		{
			HudReferenceSearching->SetCountdownText(GetWorld()->GetTimerManager().GetTimerRemaining(SearchingPhaseTimerHandle),
				UserHUDComponentSearchingInstance, searchingPhasetime);
		}
	}

	// Handle Camera FOV changes while Timer is active
	if (GetWorld()->GetTimerManager().IsTimerActive(CameraSetupTimerHandle))
	{
		// Lerp based on time remaining on timer
		float LerpAlpha = (GetWorld()->GetTimerManager().GetTimerRemaining(CameraSetupTimerHandle) / CameraSetupTime);
		TopDownCameraComponent->SetFieldOfView(FMath::Lerp(InGameCameraFOV, MainMenuCameraFOV, LerpAlpha));
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"),LerpAlpha));
	}
}

void AHideAndSeekCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Create Start Screen and Set up buttons
	if (UserHUDComponent_StartScreen)
	{
		// Set up Hiding HUD and Add to Viewport
		UserHUDComponentStartScreenInstance = CreateWidget<UUserWidget>(this->GetWorld(), UserHUDComponent_StartScreen);
		UserHUDComponentStartScreenInstance->AddToViewport();
		
		// Set up reference, so casting happens once on initiation and not in Tick()
		if (auto* hud = Cast<AHideAndSeekHUD>(UserHUDComponentStartScreenInstance->GetOwningPlayer()->GetHUD()))
		{
			hud->SetupStartButton(UserHUDComponentStartScreenInstance);
			hud->SetupQuitButton(UserHUDComponentStartScreenInstance);
		}
		
	}
	
	TopDownCameraComponent->SetFieldOfView(MainMenuCameraFOV);

	// Set up On Overlap Begin Function in Begin Play
	PlayerBoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AHideAndSeekCharacter::OnOverlapBegin);

	// Search the scene for NPCs
	TArray<AActor*> FoundNPCActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPC_Character::StaticClass(), FoundNPCActors);

	// Clear Bush Locations
	NPC_Characters.Empty();

	// Add found actors to the BushLocations Array
	for (AActor* npc : FoundNPCActors)
	{
		NPC_Characters.Add(Cast<ANPC_Character>(npc));
	}

	// Add slow camera shake for a more dynamic background
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayWorldCameraShake(
		GetWorld(), StartMenuCamShake, this->GetActorLocation(), 0.0f, 100.0f, 0.0f, true);
	
}


void AHideAndSeekCharacter::StartGame()
{
	// Remove Main Menu
	UserHUDComponentStartScreenInstance->RemoveFromParent();	

	// Disable all Camera Shakes
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StopAllCameraShakes();
	
	// Start Hiding Phase after Setting up Camera
	GetWorld()->GetTimerManager().SetTimer(CameraSetupTimerHandle,
										   this, &AHideAndSeekCharacter::StartHidingPhase,
										   CameraSetupTime, false);	
	
}

void AHideAndSeekCharacter::StartHidingPhase()
{	
	// Enable Player Controller on start of Hiding Phase
	if (auto* controller = Cast<AHideAndSeekPlayerController>(GetController()))
	{
		controller->SetPlayerControllerActivated(true);
	}

	// Create Hiding HUD
	if (UserHUDComponent_Hiding)
	{
		// Set up Hiding HUD and Add to Viewport
		UserHUDComponentHidingInstance = CreateWidget<UUserWidget>(this->GetWorld(), UserHUDComponent_Hiding);
		UserHUDComponentHidingInstance->AddToViewport();
		
		// Set up reference, so casting happens once on initiation and not in Tick()
		HudReferenceHiding = Cast<AHideAndSeekHUD>(UserHUDComponentHidingInstance->GetOwningPlayer()->GetHUD());
	}
	
	hidingPhaseActive = true;
	// On begin of the hiding phase, create a timer that calls the searching phase function after 30 seconds.
	GetWorld()->GetTimerManager().SetTimer(HidingPhaseTimerHandle,
	                                       this, &AHideAndSeekCharacter::StartSearchingPhase, hidingPhaseTime, false);
}

void AHideAndSeekCharacter::StartSearchingPhase()
{
	// Set up Searching Hud
	// Create Hiding HUD in BeginPlay
	if (UserHUDComponent_Searching)
	{
		// Remove the Hiding HUD from Viewport
		UserHUDComponentHidingInstance->RemoveFromParent();
		
		// And initiate and add Searching HUD to Viewport
		UserHUDComponentSearchingInstance = CreateWidget<UUserWidget>(this->GetWorld(), UserHUDComponent_Searching);
		UserHUDComponentSearchingInstance->AddToViewport();
		
		// Set up reference, so casting happens once on initiation and not in Tick()
		HudReferenceSearching = Cast<AHideAndSeekHUD>(UserHUDComponentSearchingInstance->GetOwningPlayer()->GetHUD());
	}

	// Activate all NPCs that were found in the scene
	for (auto npc : NPC_Characters)
	{
		if (Cast<ANPC_Character>(npc))
		{
			npc->ActivateNPC();			
		}
	}
	
	hidingPhaseActive = false;
	searchingPhaseActive = true;

	// Set up Delegate, in order to call the End Game Function with a parameter
	FTimerDelegate EndGameDelegate;
	EndGameDelegate.BindUFunction(this, "EndGame", true);

	// Create a timer for the searching phase, calling the End Game Function (with the playerWon variable set to true)
	GetWorld()->GetTimerManager().SetTimer(SearchingPhaseTimerHandle,
	                                       EndGameDelegate, searchingPhasetime, false);
}

void AHideAndSeekCharacter::EndGame(bool playerWon = false)
{
	// End both phases
	hidingPhaseActive = false;
	searchingPhaseActive = false;

	// save local variable of the time that was remaining
	float timeRemainingOnGameEnd = GetWorld()->GetTimerManager().GetTimerRemaining(SearchingPhaseTimerHandle);

	// Also clear timers
	ClearTimerHandles();

	// Pause Game when Game is over
	APlayerController* const PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController != NULL)
	{
		PlayerController->SetPause(true);
	}	
	
	// Remove the Hiding HUD from Viewport
	UserHUDComponentSearchingInstance->RemoveFromParent();

	if (!playerWon)
	{
		if (UserHUDComponent_GameOverScreen_Loss)
		{		
			// And initiate and add Searching HUD to Viewport
			UserHUDComponentEndScreenLossInstance = CreateWidget<UUserWidget>(this->GetWorld(), UserHUDComponent_GameOverScreen_Loss);
			UserHUDComponentEndScreenLossInstance->AddToViewport();

			// Set up reference, so casting happens once on initiation and not in Tick()
			if (auto* hud = Cast<AHideAndSeekHUD>(UserHUDComponentEndScreenLossInstance->GetOwningPlayer()->GetHUD()))
			{
				hud->SetupBackButton(UserHUDComponentEndScreenLossInstance);
				hud->SetupQuitButton(UserHUDComponentEndScreenLossInstance);
				hud->SetGameOverRemainingTimeText(timeRemainingOnGameEnd, UserHUDComponentEndScreenLossInstance );
			}
		}
	}
	else
	{
		if (UserHUDComponent_GameOverScreen_Won)
		{		
			// And initiate and add Searching HUD to Viewport
			UserHUDComponentEndScreenWonInstance = CreateWidget<UUserWidget>(this->GetWorld(), UserHUDComponent_GameOverScreen_Won);
			UserHUDComponentEndScreenWonInstance->AddToViewport();
			
			// Set up reference, so casting happens once on initiation and not in Tick()
			if (auto* hud = Cast<AHideAndSeekHUD>(UserHUDComponentEndScreenWonInstance->GetOwningPlayer()->GetHUD()))
			{
				hud->SetupBackButton(UserHUDComponentEndScreenWonInstance);
				hud->SetupQuitButton(UserHUDComponentEndScreenWonInstance);
			}
		}
		
	}
}

void AHideAndSeekCharacter::ClearTimerHandles()
{
	// Clearing all timers
	GetWorld()->GetTimerManager().ClearTimer(HidingPhaseTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(SearchingPhaseTimerHandle);
}

void AHideAndSeekCharacter::UpdatePlayerHidden(bool isPlayerHidden)
{
	// Simple Function to toggle if the Player can be sensed by the NPCs	
	if (isPlayerHidden)
	{
		PlayerHidden = true;
	}
	else
	{
		PlayerHidden = false;
	}
}

bool AHideAndSeekCharacter::GetPlayerHidden()
{
	return PlayerHidden;
}

void AHideAndSeekCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep,
                                           const FHitResult& SweepResult)
{
	// Player can only lose during searching phase
	if (!GetWorld()->GetTimerManager().IsTimerActive(HidingPhaseTimerHandle))
	{
		if (OtherActor->IsA(ANPC_Character::StaticClass()) && !PlayerHidden)
		{
			// If the overlapping actor is a NPC and the Player is not hidden		
			EndGame(false);
		}
	}
}

void AHideAndSeekCharacter::RotateCamera(float direction)
{
	// Rotate Camera Boom if either Hiding or Searching Phase are active
	if ((GetWorld()->GetTimerManager().IsTimerActive(HidingPhaseTimerHandle))
		|| (GetWorld()->GetTimerManager().IsTimerActive(SearchingPhaseTimerHandle)))
	{		
		CameraBoom->AddWorldRotation(FRotator(0.0f, direction, 0.f));
	}
}

