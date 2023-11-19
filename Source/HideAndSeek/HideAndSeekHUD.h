// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/Button.h"
#include "HideAndSeekHUD.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API AHideAndSeekHUD : public AHUD
{
	GENERATED_BODY()

	AHideAndSeekHUD();

public:
	
	void SetCountdownText(float remainingTime, UUserWidget* widget, float maximumTime);
	void SetGameOverRemainingTimeText(float remainingTime, UUserWidget* widget);

	UButton* SearchWidgetTree(UUserWidget *widget, FString buttonName);

	// Functions to setup buttons (initiating all in one go causes problems)
	void SetupStartButton(UUserWidget* widget);
	void SetupQuitButton(UUserWidget* widget);
	void SetupBackButton(UUserWidget* widget);
	
	// Add reference to the Text Block to change the countdown text during Hiding Phase
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* CountdownTextBlock;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TimeRemainingTextBlock;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* CountdownProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;
	
	// On Clicked Functions for all Buttons
	
	UFUNCTION()
	void StartButtonClicked();
	
	UFUNCTION()
	void QuitButtonClicked();
	
	UFUNCTION()
	void BackButtonClicked();
};
