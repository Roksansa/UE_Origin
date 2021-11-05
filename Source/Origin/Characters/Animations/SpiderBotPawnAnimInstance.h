// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BasePawnAnimInstance.h"
#include "SpiderBotPawnAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API USpiderBotPawnAnimInstance : public UBasePawnAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Config | IK Setting")
	FVector RightFrontFootEffectorLocation = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Config | IK Setting")
	FVector RightRearFootEffectorLocation = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Config | IK Setting")
	FVector LeftFrontFootEffectorLocation = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Config | IK Setting")
	FVector LeftRearFootEffectorLocation = FVector::ZeroVector;
	
private:
	TWeakObjectPtr<class ASpiderBotPawn> CurrentPawn;
};
