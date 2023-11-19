// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_AIController.h"

#include "HideAndSeekCharacter.h"
#include "NPC_Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ANPC_AIController::ANPC_AIController(const FObjectInitializer& ObjectInitializer)
{
	PerceptionSetup();
}

void ANPC_AIController::PerceptionSetup()
{
	// Create Perception Component
	AI_SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AI Sight Config"));
	if (AI_SightConfig)
	{
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
		// Sight radius at which player can be seen
		AI_SightConfig->SightRadius = 500.0f;

		// Degrees the NPC can see
		AI_SightConfig->PeripheralVisionAngleDegrees = 90.0f;

		// How long the NPC is alerted, before giving up
		// Setting this to 0 means the NPC will never give up
		AI_SightConfig->SetMaxAge(0.0f);

		// Detect all, as there are no other NPCs in the game
		AI_SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		AI_SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		AI_SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	
		GetPerceptionComponent()->SetDominantSense(*AI_SightConfig->GetSenseImplementation());

		// Set up function that is called when the player is detected
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ANPC_AIController::OnTargetDetected);

		// Apply config
		GetPerceptionComponent()->ConfigureSense(*AI_SightConfig);
	}
}

void ANPC_AIController::SetNPCActive(bool isActive)
{
	isNPCActive = isActive;
}

void ANPC_AIController::OnTargetDetected(AActor* Actor, const FAIStimulus Stimulus)
{
	if (isNPCActive)
	{
		// Check if sensed Actor is the player
		if (auto* const character = Cast<AHideAndSeekCharacter>(Actor))
		{
			if (!Cast<AHideAndSeekCharacter>(Actor)->GetPlayerHidden())
			{
				// If player is not hidden, cast to character and chase player
				if (auto* NPC_Pawn = Cast<ANPC_Character>(GetPawn()))
				{
					// If Controller is attached to NPC_Character, start chasing Player
					NPC_Pawn->ChasePlayer(character);
				}
			}
		}
	}
}
