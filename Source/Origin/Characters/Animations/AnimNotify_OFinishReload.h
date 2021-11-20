// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_OFinishReload.generated.h"


UCLASS(const, hidecategories=Object, collapsecategories, meta=(DisplayName="Finish Reload"))
class ORIGIN_API UAnimNotify_OFinishReload : public UAnimNotify
{
	GENERATED_BODY()
		
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	int32 CountBullets;
};
