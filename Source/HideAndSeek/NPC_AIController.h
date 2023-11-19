// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NPC_AIController.generated.h"

UCLASS()
class HIDEANDSEEK_API ANPC_AIController : public AAIController
{
	GENERATED_BODY()

public:
	explicit ANPC_AIController(const FObjectInitializer& ObjectInitializer);

	void PerceptionSetup();

	void SetNPCActive(bool isActive);

private:
	class UAISenseConfig_Sight* AI_SightConfig;

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, const FAIStimulus Stimulus);

	bool isNPCActive = false;
};
