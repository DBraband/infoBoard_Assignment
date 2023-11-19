// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HideAndSeekHUD.h"
#include "NPC_Character.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "HideAndSeekCharacter.generated.h"

UCLASS(Blueprintable)
class AHideAndSeekCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHideAndSeekCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	// Used to set up scene and camera (coming from the main menu)
	void StartGame();
	
	// Start the hiding phase after everything is set up
	void StartHidingPhase();

	// Start the searching phase after the timer has ended
	void StartSearchingPhase();

	UFUNCTION()
	void EndGame(bool playerWon);

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// Function to toggle if player can be seen by NPCs
	void UpdatePlayerHidden(bool isPlayerHidden);
	bool GetPlayerHidden();

	// Box Collider Setup to determine if Player got found
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Behavior, meta=(AllowPrivateAccess = "true"))
	UBoxComponent* PlayerBoxCollider;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);

	// Array of all NPCs in the scene to enable them once the Searching Phase starts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NPC, meta=(AllowPrivateAccess = "true"))	
	TArray<ANPC_Character*> NPC_Characters;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> StartMenuCamShake;

	void RotateCamera(float direction);


private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// The FOV settings for the camera in the menu and in game
	float MainMenuCameraFOV = 40.0f;
	float InGameCameraFOV = 15.0f;

	// Camera Timer Settings

	// Timer handle to keep track of the camera setup (changing the FOV after starting)
	FTimerHandle CameraSetupTimerHandle;
	// Time it takes for the FOV to change
	float CameraSetupTime = 0.2f;

	/** User Widget Component for the Searching Phase */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> UserHUDComponent_StartScreen;
	UUserWidget* UserHUDComponentStartScreenInstance;

	/** User Widget Component for the Searching Phase */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> UserHUDComponent_GameOverScreen_Won;
	UUserWidget* UserHUDComponentEndScreenWonInstance;

	/** User Widget Component for the Searching Phase */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> UserHUDComponent_GameOverScreen_Loss;
	UUserWidget* UserHUDComponentEndScreenLossInstance;

	/** User Widget Component for the Hiding Phase */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> UserHUDComponent_Hiding;
	UUserWidget* UserHUDComponentHidingInstance;

	/** User Widget Component for the Searching Phase */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> UserHUDComponent_Searching;
	UUserWidget* UserHUDComponentSearchingInstance;

	// References to later change the On screen Text through this class in Tick
	AHideAndSeekHUD* HudReferenceHiding;
	AHideAndSeekHUD* HudReferenceSearching;

	void ClearTimerHandles();

	/** Timer Handle for the Hiding Phase*/
	FTimerHandle HidingPhaseTimerHandle;

	/** Timer Handle for the Hiding Phase*/
	FTimerHandle SearchingPhaseTimerHandle;

	/** Booleans to determine which phase is active, to keep track of the timer in Tick*/
	bool hidingPhaseActive = false;
	bool searchingPhaseActive = false;

	// Bool to determine when the Player is Hidden
	bool PlayerHidden = false;

	// Hiding Phase length in seconds
	float hidingPhaseTime = 30.0f; // Standard: 30 seconds

	// Searching Phase length in seconds
	float searchingPhasetime = 300.0f; // Standard: 300 seconds (5 minutes)
};
