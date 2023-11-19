// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_Character.h"

#include "AIController.h"
#include "HideAndSeekCharacter.h"
#include "NavigationSystem.h"
#include "NPC_AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISenseConfig_Sight.h"

// Sets default values
ANPC_Character::ANPC_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set Standard Movement Settings 
	GetCharacterMovement()->MaxWalkSpeed = standardWalkSpeed;
}

// Called when the game starts or when spawned
void ANPC_Character::BeginPlay()
{
	Super::BeginPlay();

	// Search the scene for Bush Actors
	TArray<AActor*> FoundBushActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABushActor::StaticClass(), FoundBushActors);

	// Clear Bush Locations
	BushLocations.Empty();

	// Add found actors to the BushLocations Array
	for (AActor* FoundBushActor : FoundBushActors)
	{
		BushLocations.Add(Cast<ABushActor>(FoundBushActor));
	}
}

// Called every frame
void ANPC_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANPC_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANPC_Character::ActivateNPC()
{
	// Activate NPC when the Searching Phase starts

	if (ANPC_AIController* NPCController = Cast<ANPC_AIController>(GetController()))
	{		
		NPCController->SetNPCActive(true);
	}
	
	// start cycle by going to random Location
	GoToRandomLocation();
}

void ANPC_Character::GoToNextBush()
{
	// Clear Previous Timer
	GetWorldTimerManager().ClearTimer(NPCActionDelayHandle);

	// Get AI Controller
	ANPC_AIController* NPCController = Cast<ANPC_AIController>(GetController());

	// Move to random BushLocation based on Array
	int randomInt = FMath::RandRange(0, BushLocations.Num() - 1);
	NPCController->MoveToActor(BushLocations[randomInt]);

	// Set up delay before moving to next location

	// Generate random delay in range
	float delay = FMath::FRandRange(BushCheckDelayMIN, BushCheckDelayMAX);

	// Call timer with delay to go to a random location next
	GetWorldTimerManager().SetTimer(NPCActionDelayHandle, this, &ANPC_Character::GoToRandomLocation, delay, false);
}

void ANPC_Character::GoToRandomLocation()
{
	// Clear Previous Timer
	GetWorldTimerManager().ClearTimer(NPCActionDelayHandle);

	if (Cast<ANPC_AIController>(GetController()))
	{
		ANPC_AIController* NPCController = Cast<ANPC_AIController>(GetController());
		FVector Origin = GetActorLocation();

		// Get Nav System
		if (auto* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
		{
			FNavLocation NavLocation;
			// Search for the random point, if a point was found, save the location to local variable
			if (NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, NavLocation))
			{
				NPCController->MoveToLocation(NavLocation);
			}
	}
	

	// Set up delay before moving to next Bush

	// Generate random delay in range
	float delay = FMath::FRandRange(RandomLocationCheckDelayMIN, RandomLocationCheckDelayMAX);

	// Call timer with delay to go to a random location next
	GetWorldTimerManager().SetTimer(NPCActionDelayHandle, this, &ANPC_Character::GoToNextBush, delay, false);
	}
}

void ANPC_Character::ClearTimerHandles()
{
	// Clearing all timers
	GetWorld()->GetTimerManager().ClearTimer(NPCActionDelayHandle);
}

void ANPC_Character::ChasePlayer(AHideAndSeekCharacter* PlayerReference)
{
	/**
	 * This function stops all other timers and NPC behavior,
	 * and only chases the player once they have been seen once.
	 */

	// Clear all timers
	ClearTimerHandles();

	// Update Movement Settings
	GetCharacterMovement()->MaxWalkSpeed = chasingWalkSpeed;

	// Get AI Controller
	ANPC_AIController* NPCController = Cast<ANPC_AIController>(GetController());

	// Move to Player
	NPCController->MoveToActor(PlayerReference);
}