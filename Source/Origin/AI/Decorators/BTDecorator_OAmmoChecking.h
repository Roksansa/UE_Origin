// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapon/OBaseWeapon.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_OAmmoChecking.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UBTDecorator_OAmmoChecking : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_OAmmoChecking();
protected:
	/**count < WeaponAmmo = true*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	int32 Count = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	EOAmmoType Type;
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
