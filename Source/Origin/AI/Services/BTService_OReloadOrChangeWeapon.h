// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_OReloadOrChangeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UBTService_OReloadOrChangeWeapon : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_OReloadOrChangeWeapon();
protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
