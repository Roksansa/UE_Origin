// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "BasePawnAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UBasePawnAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Config")
	float InputForward = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Config")
	float InputRight = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Config")
	bool bIsOnAir = false;
	
private:
	TWeakObjectPtr<class AGameCodeBasePawn> CurrentPawn;
};
