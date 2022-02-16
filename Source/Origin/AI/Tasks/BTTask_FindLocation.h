// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindLocation.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UBTTask_FindLocation : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_FindLocation();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float Radius = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FBlackboardKeySelector LocationKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bPointAroundSelf = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI", meta=(EditCondition="!bPointAroundSelf"))
	FBlackboardKeySelector PointKey;
};
