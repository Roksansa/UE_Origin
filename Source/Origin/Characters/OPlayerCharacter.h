// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OBaseCharacter.h"
#include "OPlayerCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAllowFire, bool);

UCLASS(Blueprintable)
class ORIGIN_API AOPlayerCharacter : public AOBaseCharacter
{
	GENERATED_BODY()

public:

	AOPlayerCharacter();
	
	virtual void BeginPlay() override;
	
	virtual void LookUp(float Value) override;
	virtual void Turn(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void MoveForward(float Value) override;

	virtual void TurnAtRate(float Value) override;
	virtual void LookUpAtRate(float Value) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnStartCrawling(float HalfHeightAdjust, float ScaledHeightAdjust) override;
	virtual void OnEndCrawling(float HalfHeightAdjust, float ScaledHeightAdjust) override;
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

	virtual void MoveSwimRight(float Value) override;
	virtual void MoveSwimForward(float Value, float LastSwimUpValue) override;
	virtual void MoveSwimUp(float Value) override;

	virtual void OnStartSwimming(float HalfHeightAdjust, float ScaledHeightAdjust) override;
	virtual void OnEndSwimming(float HalfHeightAdjust, float ScaledHeightAdjust) override;

	virtual void ClimbLadder(float Value) override;
	virtual void InteractionWithLadder() override;

	virtual void StartFire() override;
	virtual void StopFire() override;

	float GetViewPitchMin() const;
	float GetViewPitchMax() const;

	FOnAllowFire OnAllowFire;
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	float ViewPitchMin = -89.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	float ViewPitchMax = 15.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Crawling")
	float LocationXMeshOffsetForCrawling = 50.f;

private:
	bool bWantFire = false;
};
