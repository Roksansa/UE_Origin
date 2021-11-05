// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "BasePawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UBasePawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
public:

	void JumpStart();

	virtual bool IsFalling() const override;
	
protected:
	UPROPERTY(EditAnywhere)
	float MaxSpeed = 1000.f;
	
	UPROPERTY(EditAnywhere)
	bool bEnableGravity;

	UPROPERTY(EditAnywhere)
	float InitialJumpVelocity = 1000.f;
	
private:
	FVector VerticalVelocity = FVector::ZeroVector;
	
	bool bIsFalling = false;
};
