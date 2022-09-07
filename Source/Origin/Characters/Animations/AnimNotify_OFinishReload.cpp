// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_OFinishReload.h"

#include "Characters/OBaseCharacter.h"
#include "Components/OWeaponComponent.h"

void UAnimNotify_OFinishReload::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AOBaseCharacter* BaseCharacter = Cast<AOBaseCharacter>(MeshComp->GetOwner());
	if (BaseCharacter)
	{
		BaseCharacter->GetWeaponComponent()->GetOnNotifyFinishReload().Broadcast(CountBullets, true);
	}
	Super::Notify(MeshComp, Animation);
}
