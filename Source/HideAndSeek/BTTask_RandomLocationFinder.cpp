// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RandomLocationFinder.h"

#include "NavigationSystem.h"
#include "NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_RandomLocationFinder::UBTTask_RandomLocationFinder(const FObjectInitializer& ObjectInitializer)
{
	NodeName = "Find random location in NavMesh.";
}

EBTNodeResult::Type UBTTask_RandomLocationFinder::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get owning AI Controller
	if (auto* const controller = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
	{
		if (auto* const npc = controller->GetPawn())
		{
			// Get current Actor Location
			const auto Origin = npc->GetActorLocation();

			// Get Nav System
			if (auto* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				FNavLocation NavLocation;
				// Search for the random point, if a point was found, save the location to local variable
				if (NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, NavLocation))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(
						GetSelectedBlackboardKey(), NavLocation.Location);
				}

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}
