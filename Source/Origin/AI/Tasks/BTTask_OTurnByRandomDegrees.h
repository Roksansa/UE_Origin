// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_OTurnByRandomDegrees.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UBTTask_OTurnByRandomDegrees : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_OTurnByRandomDegrees();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	int32 MinDegrees = 45;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	int32 MaxDegrees = 360;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
