// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RandomLocationFinder.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API UBTTask_RandomLocationFinder : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	explicit UBTTask_RandomLocationFinder(const FObjectInitializer& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	float SearchRadius = 1800.0f;
};
