// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindLocation.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindLocation::UBTTask_FindLocation()
{
	NodeName = "Find Location";
}

EBTNodeResult::Type UBTTask_FindLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Controller || !Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Pawn);
	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}
	
	FNavLocation ResultLocation;
	FVector Location = Pawn->GetActorLocation();
	if (!bPointAroundSelf)
	{
		AActor* FoundActor = Cast<AActor>(Blackboard->GetValueAsObject(PointKey.SelectedKeyName));
		if (!FoundActor)
		{
			return EBTNodeResult::Failed;
		}
		Location = FoundActor->GetActorLocation();
	}
	const bool bFound = NavSystem->GetRandomReachablePointInRadius(Location, Radius, ResultLocation);
	if (!bFound)
	{
		return EBTNodeResult::Failed;
	}
	
	Blackboard->SetValueAsVector(LocationKey.SelectedKeyName, ResultLocation.Location);
	return EBTNodeResult::Succeeded;
}
