// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_OFire.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/OBaseCharacter.h"
#include "Characters/Controllers/OAIController.h"

UBTService_OFire::UBTService_OFire()
{
	NodeName ="Fire enemy";
}

void UBTService_OFire::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const AOAIController* Controller = Cast<AOAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	
	const bool bWantFire = Blackboard && Blackboard->GetValueAsObject(EnemyKey.SelectedKeyName);
	AOBaseCharacter* Character = Cast<AOBaseCharacter>(Controller->GetPawn());
	if (Character && !Character->IsDie())
	{
		bWantFire ? Character->StartFire() : Character->StopFire();
	}
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
