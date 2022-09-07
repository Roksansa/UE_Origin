// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_OFinishEquip.generated.h"

UCLASS(const, hidecategories=Object, collapsecategories, meta=(DisplayName="Finish Equip"))
class ORIGIN_API UAnimNotify_OFinishEquip : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
protected:
	// Name of notify that is passed to the PlayMontage K2Node.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	bool bIsOldWeaponFinishPosition;
};
