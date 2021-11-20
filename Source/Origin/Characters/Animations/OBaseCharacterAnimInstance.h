// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTypes.h"
#include "Animation/AnimInstance.h"
#include "OBaseCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UOBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
protected:
	void CalculateIKFootsOffset(float DeltaSeconds);
	void CalculateSlopeFloorForCurrentPawn(float DeltaSeconds);
	void CalculateOffsetRootBoneInZ(float DeltaSeconds, const float OffsetForFoot);
	void CalculateIKFootsOffsetOnLadder(float DeltaSeconds);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bIsFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bIsCrouching = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bIsSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bIsCrawling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bIsOutOfStaminaState = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bIsSwimming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bIsSwimmingOnSurface = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bIsOnLadder = false;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Settings|IK Setting")
	FVector IKRightFootEffectorLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Settings|IK Setting")
	FVector IKLeftFootEffectorLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Settings|IK Setting")
	FVector IKRightHandEffectorLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Settings|IK Setting")
	FVector IKLeftHandEffectorLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	float OffsetZLocationForCrawl = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	FVector OffsetTransformBoneIdleCrouch = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	FRotator OffsetRotatorBoneCrouch = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float ClimbingLadderSpeedRatio = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float Direction = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	FRotator AimRotation = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float MinAimPitch = -15.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float MaxAimPitch = -45.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float MinAimYawIn = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float MaxAimYawIn = 90.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	float CurrentPitch = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float LerpSpeed = 0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	EOEquippableItemType WeaponItemType = EOEquippableItemType::None;
private:
	TWeakObjectPtr<class AOBaseCharacter> CurrentCharacter;
	TWeakObjectPtr<class UOCharacterIKComponent> IKComp;

	float IKRightFootOffset;

	float IKLeftFootOffset;

	FVector IKRightHandOffset;

	FVector IKLeftHandOffset;

	float LastZLocationForCharacter = 0.f;

	//20 point up for current animation
	float MaxDistanceLegUpAfterChangeZLocationByCrouched = -20.f;

	bool bCheckedMeshPositionOnLadder = false;
	float PositionMeshOnLadderOffset = 0.f;
	float PositionOnLadderOffset = 0.f;

	void CalcDirection();

	void CalcAimRotation(const FRotator& Rotator);
};
