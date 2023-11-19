// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindNextBush.h"

#include "NavigationSystem.h"
#include "NPC_AIController.h"
#include "NPC_Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindNextBush::UBTTask_FindNextBush(const FObjectInitializer& ObjectInitializer)
{
	NodeName = "Find the next Bush to check.";
}

EBTNodeResult::Type UBTTask_FindNextBush::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
				// Get Next Bush Location from random selection of Array
				ANPC_Character* const NPCCharacter = Cast<ANPC_Character>(npc);

				if (NPCCharacter->BushLocations.Num() > 0)
				{
					int randomInt = FMath::RandRange(0, NPCCharacter->BushLocations.Num() - 1);

					// Create Query Params to test the path before returning the path location
					// (Should always be true, as bushes are located only in reachable locations)
					FPathFindingQuery QueryParams;
					QueryParams.StartLocation = Origin;
					QueryParams.EndLocation = NPCCharacter->BushLocations[randomInt]->GetActorLocation();
					QueryParams.Owner = this;
					QueryParams.SetAllowPartialPaths(false);

					// If Target is reachable, return location to blackboard
					// if(NavSys->TestPathSync(QueryParams, EPathFindingMode::Hierarchical, 0))
					// {
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(),
					                                                     NPCCharacter->BushLocations[1]->
					                                                     GetActorLocation());

					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange,
					                                 FString::Printf(TEXT("My Location is: %s"),
					                                                 *NPCCharacter->BushLocations[randomInt]->
					                                                  GetActorLocation().ToString()));


					// Return Task as Success
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
					//}
				}
			}
		}
	}
	// If not returned as Success, return as Failed
	return EBTNodeResult::Failed;
}
