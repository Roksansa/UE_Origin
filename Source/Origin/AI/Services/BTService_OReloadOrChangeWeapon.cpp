// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_OReloadOrChangeWeapon.h"

#include "Characters/OAICharacter.h"
#include "Characters/Controllers/OAIController.h"

UBTService_OReloadOrChangeWeapon::UBTService_OReloadOrChangeWeapon()
{
	NodeName = "Reload Or Change Weapon";
}

void UBTService_OReloadOrChangeWeapon::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const AOAIController* Controller = Cast<AOAIController>(OwnerComp.GetAIOwner());
	AOAICharacter* Character = Cast<AOAICharacter>(Controller->GetPawn());
	if (Character && !Character->IsDie())
	{
		Character->ReloadAmmo();
	}
}