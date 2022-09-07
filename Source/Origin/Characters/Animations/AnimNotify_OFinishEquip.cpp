// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_OFinishEquip.h"

#include "Characters/OBaseCharacter.h"
#include "Components/OWeaponComponent.h"

void UAnimNotify_OFinishEquip::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AOBaseCharacter* BaseCharacter = Cast<AOBaseCharacter>(MeshComp->GetOwner());
	if (BaseCharacter)
	{
		BaseCharacter->GetWeaponComponent()->GetOnNotifyFinishEquip().Broadcast(bIsOldWeaponFinishPosition);
	}
	Super::Notify(MeshComp, Animation);
}
