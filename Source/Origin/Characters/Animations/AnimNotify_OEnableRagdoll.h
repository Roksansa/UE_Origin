// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_OEnableRagdoll.generated.h"

/**
 * 
 */
UCLASS(const, hidecategories=Object, collapsecategories, meta=(DisplayName="Enable Ragdol"))
class ORIGIN_API UAnimNotify_OEnableRagdoll : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
