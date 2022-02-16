// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_OFindEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UBTService_OFindEnemy : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_OFindEnemy();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FBlackboardKeySelector EnemyKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
