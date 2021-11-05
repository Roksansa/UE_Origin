// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OBaseWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class ORIGIN_API AOBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	AOBaseWeapon();

	virtual void Fire();

	/**Add check trace from muzzle - set on spawn weapon from weapon component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	bool bComplexTrace = false;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	float TraceMaxDistance = 1500.f;

	virtual void BeginPlay() override;

	void MakeShot();

private:
	APlayerController* GetPlayerController() const;
	bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;
	FVector GetMuzzleWorldLocation() const;
	bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;
	void MakeHit(FHitResult& HitResult, const FVector TraceStart, const FVector TraceEnd) const;
};
