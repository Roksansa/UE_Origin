// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_OFindEnemy.h"

#include "AI/Components/OAIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Controllers/OAIController.h"

UBTService_OFindEnemy::UBTService_OFindEnemy()
{
	NodeName = "Find Enemy";
}

void UBTService_OFindEnemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const AOAIController* Controller = Cast<AOAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Controller && Blackboard && Controller->GetAIPerceptionComponent())
	{
		Blackboard->SetValueAsObject(EnemyKey.SelectedKeyName, Controller->GetAIPerceptionComponent()->GetNearestActor());
	}
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
