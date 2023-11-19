// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BushActor.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionTypes.h"
#include "NPC_Character.generated.h"

UCLASS()
class HIDEANDSEEK_API ANPC_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC_Character();

	// BushLocations to randomly pick from when searching for the Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Behavior, meta=(AllowPrivateAccess = "true"))	
	TArray<ABushActor*> BushLocations;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ChasePlayer(AHideAndSeekCharacter* PlayerReference);

	void ActivateNPC();

private:
	float standardWalkSpeed = 300.0f;
	float chasingWalkSpeed = 800.0f;

	// Move Functions

	// Find next Bush to Check
	void GoToNextBush();

	// Find random location to patrol
	void GoToRandomLocation();
	void ClearTimerHandles();
	float SearchRadius = 800.0f;

	// Handle the delays between interactions
	FTimerHandle NPCActionDelayHandle;

	// Set up range of random delay between actions
	float BushCheckDelayMIN = 6.0f;
	float BushCheckDelayMAX = 8.0f;
	float RandomLocationCheckDelayMIN = 2.0f;
	float RandomLocationCheckDelayMAX = 8.0f;
};
