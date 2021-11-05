// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OCharacterIKComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ORIGIN_API UOCharacterIKComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOCharacterIKComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//begin IK settings
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Config|IK Setting")
	FName RightFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Config|IK Setting")
	FName LeftFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Config|IK Setting")
	FName RightHandSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Config|IK Setting")
	FName LeftHandSocketName;

	//Distance leg beyond the capsule distance + leg lift distance (for real capsule scale)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Config|IK Setting")
	int IKTraceRealDistance = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Config|IK Setting", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKInterpSpeed = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Config|IK Setting", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKLegLiftDistance = 60.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Config|IK Setting")
	float IKLegLiftRealDistance = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Config|IK Setting", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKLegBeyondCapsuleDistance = 30.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Config|IK Setting")
	float IKLegBeyondCapsuleRealDistance = 30.f;

	/** Don't change without any reason */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Config|IK Setting")
	FVector HalfSizeBoxForSweepLadder = FVector(5.f, 20.f, 0.01f);
	/** Don't change without any reason */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Config|IK Setting")
	FVector HalfSizeBoxForSweep = FVector(5.f, 14.f, 0.01f);

	/** Don't change without any reason */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Config|IK Setting")
	float MinFootOffsetResultZ = 30.f;
	/** Don't change without any reason */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Config|IK Setting")
	float MaxFootOffsetResultZ = 60.f;
	/** Don't change without any reason */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Config|IK Setting")
	float MinFootOffsetResultX = 10.f;
	/** Don't change without any reason */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Config|IK Setting")
	float MaxFootOffsetResultX = 30.f;

	//end IK settings

public:
	float GetIKOffsetForSocketName(const FName& SocketName) const;
	float GetIKInterpSpeed() const;

	FVector GetIKOffsetFromLadderStepForSocketName(const FName& SocketName) const;
	FVector GetIKHandOffsetFromLadderStepForSocketName(const FName& SocketName) const;

	FName GetRightFootSocketName() const;
	FName GetLeftFootSocketName() const;
	FName GetRightHandSocketName() const;
	FName GetLeftHandSocketName() const;

private:
	float IKScale = 1.f;
	float IKFootCorrection = 3.f;

	TWeakObjectPtr<class AOBaseCharacter> CachedCharacter;
	TWeakObjectPtr<class UOBaseCharacterMovementComponent> CachedMovComp;
};
