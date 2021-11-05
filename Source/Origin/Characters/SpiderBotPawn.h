// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameCodeBasePawn.h"
#include "SpiderBotPawn.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API ASpiderBotPawn : public AGameCodeBasePawn
{
	GENERATED_BODY()
public:
	ASpiderBotPawn();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetIKRightFrontFootOffset() const { return IKRightFrontFootOffset;}
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetIKRightRearFootOffset() const { return IKRightRearFootOffset;}
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetIKLeftFrontFootOffset() const { return IKLeftFrontFootOffset;}
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetIKLeftRearFootOffset() const { return IKLeftRearFootOffset;}
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Config|IK Setting")
	FName RightFrontFootSocketName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Config|IK Setting")
	FName RightRearFootSocketName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Config|IK Setting")
	FName LeftFrontFootSocketName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Config|IK Setting")
	FName LeftRearFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Config|IK Setting")
	int IKTraceDistance = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Config|IK Setting", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKExtendDistance = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Config|IK Setting", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKInterpSpeed = 30.f;

private:
	float GetIKOffsetForSocketName(const FName& SocketName) const;
	
	float IKRightFrontFootOffset = 0.f;
	float IKRightRearFootOffset = 0.f;
	float IKLeftFrontFootOffset = 0.f;
	float IKLeftRearFootOffset = 0.f;

	float IKScale = 1.f;;
};
