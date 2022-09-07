// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_OHealthPercent.h"

#include "AIController.h"
#include "Characters/OAICharacter.h"
#include "Components/OPrimaryAttributesComponent.h"

UBTDecorator_OHealthPercent::UBTDecorator_OHealthPercent()
{
	NodeName = "Check Health Percent";
}

bool UBTDecorator_OHealthPercent::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	AOAICharacter* Character = Cast<AOAICharacter>(Controller->GetPawn());
	if (Character && !Character->IsDie())
	{
		const float CurrentHealthPercent = Character->GetPrimaryAttributesComponent()->GetCurrentHealthPercent();
		return CurrentHealthPercent < (Percent/100.f);
	}
	return false;
}
