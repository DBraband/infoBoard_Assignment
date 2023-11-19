// Fill out your copyright notice in the Description page of Project Settings.


#include "HideAndSeekHUD.h"

#include "HideAndSeekCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AHideAndSeekHUD::AHideAndSeekHUD()
{
}

void AHideAndSeekHUD::SetCountdownText(float remainingTime, UUserWidget* widget, float maximumTime)
{
	// Change Text Box content
	CountdownTextBlock = (UTextBlock*)widget->GetWidgetFromName(TEXT("CountdownText"));
	FString RemainingTimeString = FString::SanitizeFloat(FMath::Floor(remainingTime));
	RemainingTimeString.RemoveFromEnd(TEXT(".0"));
	CountdownTextBlock->SetText(FText::FromString(RemainingTimeString));

	// Progress Bar
	CountdownProgressBar = (UProgressBar*)widget->GetWidgetFromName(TEXT("ProgressBar"));
	CountdownProgressBar->SetPercent(remainingTime/maximumTime);
}

void AHideAndSeekHUD::SetGameOverRemainingTimeText(float remainingTime, UUserWidget* widget)
{	
	// Change Text Box content
	TimeRemainingTextBlock = (UTextBlock*)widget->GetWidgetFromName(TEXT("TimeRemainingTextBlock"));
	FString RemainingTimeString = FString::SanitizeFloat(FMath::Floor(remainingTime));
	RemainingTimeString.RemoveFromEnd(TEXT(".0"));
	TimeRemainingTextBlock->SetText(FText::FromString(RemainingTimeString));
}

UButton* AHideAndSeekHUD::SearchWidgetTree(UUserWidget* widget, FString buttonName)
{
	UButton* FoundButton = nullptr;  // Declare a variable to store the result

	widget->WidgetTree->ForEachWidget([&](UWidget* Widget) {
		check(Widget);

		if (Widget->IsA(UButton::StaticClass()))
		{
			UButton* Button = Cast<UButton>(Widget);

			if (Button)
			{
				if (Button->GetName() == buttonName)
				{
					FoundButton = Button; 
					return false; 
				}
			}
		}
		return true;
	});

	return FoundButton;
}

void AHideAndSeekHUD::SetupStartButton(UUserWidget* widget)
{
	StartButton = SearchWidgetTree(widget, "StartButton");
	if (StartButton != nullptr)
	{
		StartButton->OnClicked.AddDynamic(this, &AHideAndSeekHUD::StartButtonClicked);
	}
}

void AHideAndSeekHUD::SetupQuitButton(UUserWidget* widget)
{
	QuitButton = SearchWidgetTree(widget, "QuitButton");
	if (QuitButton != nullptr)
	{
		QuitButton->OnClicked.AddDynamic(this, &AHideAndSeekHUD::QuitButtonClicked);
	}
}

void AHideAndSeekHUD::SetupBackButton(UUserWidget* widget)
{
	BackButton = SearchWidgetTree(widget, "BackButton");
	if (BackButton != nullptr)
	{
		BackButton->OnClicked.AddDynamic(this, &AHideAndSeekHUD::BackButtonClicked);
	}
}


void AHideAndSeekHUD::StartButtonClicked()
{
	if (auto* Player = Cast<AHideAndSeekCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0)))
	{
		Player->StartGame();
	}	
}

void AHideAndSeekHUD::QuitButtonClicked()
{
	// Quit the Game
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}

void AHideAndSeekHUD::BackButtonClicked()
{
	// Restart Game by opening this level again (without bAbsolute, so everything gets reset)
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}
