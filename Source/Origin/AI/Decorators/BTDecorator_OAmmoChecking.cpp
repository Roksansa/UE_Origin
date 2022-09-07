// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_OAmmoChecking.h"

#include "AIController.h"
#include "Characters/OAICharacter.h"

UBTDecorator_OAmmoChecking::UBTDecorator_OAmmoChecking()
{
	NodeName = "Ammo Checking";
}

bool UBTDecorator_OAmmoChecking::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	AOAICharacter* Character = Cast<AOAICharacter>(Controller->GetPawn());
	if (Character && !Character->IsDie())
	{
		const auto Weapon = Character->GetWeaponComponent();
		const auto CurrentAmmo = Weapon->GetAmmoDescriptionByType(Type, false);
		if (CurrentAmmo.Infinity)
		{
			return false;
		}
		const auto DefaultAmmo = Weapon->GetAmmoDescriptionByType(Type, true);
		return CurrentAmmo.BulletsCount < Count && CurrentAmmo.BulletsCount < DefaultAmmo.BulletsCount;
	}
	return false;
}
