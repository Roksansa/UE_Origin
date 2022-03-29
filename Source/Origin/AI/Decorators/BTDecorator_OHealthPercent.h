// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_OHealthPercent.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UBTDecorator_OHealthPercent : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_OHealthPercent();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float Percent = 30.f;
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
