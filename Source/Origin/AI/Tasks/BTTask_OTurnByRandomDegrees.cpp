// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_OTurnByRandomDegrees.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Rotator.h"
#include "Characters/Controllers/OAIController.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_OTurnByRandomDegrees::UBTTask_OTurnByRandomDegrees()
{
	NodeName = "Turn By Random Degrees";

	BlackboardKey.AddRotatorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_OTurnByRandomDegrees, BlackboardKey));
}

EBTNodeResult::Type UBTTask_OTurnByRandomDegrees::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AOAIController* Controller = Cast<AOAIController>(OwnerComp.GetAIOwner());

	if (!Controller || !Controller->GetPawn() || !OwnerComp.GetBlackboardComponent())
	{
		return EBTNodeResult::Failed;
	}
	
	EBTNodeResult::Type Result = EBTNodeResult::Succeeded;

	APawn* Pawn = Controller->GetPawn();

	const float Degrees = FMath::RandRange(MinDegrees, MaxDegrees);
	const bool Sign = FMath::RandBool();

	FRotator CurrentRot = Pawn->GetActorRotation();
	CurrentRot = CurrentRot.GetInverse();
	FRotator NeedRot = FRotator(0, Sign*Degrees, 0);
	NeedRot = NeedRot.GetInverse();
	FRotator ResultRot = UKismetMathLibrary::ComposeRotators(CurrentRot, NeedRot);
	ResultRot = ResultRot.GetInverse();
	UE_LOG(LogTemp, Display, TEXT("Reload %s --- %s"), *OwnerComp.GetOwner()->GetName(), *ResultRot.ToString());
	UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Rotator::StaticClass())
	{
		MyBlackboard->SetValueAsRotator(BlackboardKey.SelectedKeyName, ResultRot);
	}
	else
	{
		return EBTNodeResult::Failed;
	}
	return Result;
}


