// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapon/OBaseWeapon.h"
#include "ORifleWeapon.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ORIGIN_API AORifleWeapon : public AOBaseWeapon
{
	GENERATED_BODY()

public:
	virtual void StartFire() override;
	virtual void StopFire() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|WeaponParams")
	float TimerBetweenShots = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|WeaponParams")
	float BulletSpread = 1.f;

	virtual void MakeShot() override;
	virtual FVector GetShootDirection(const FVector& ViewRotationVector) const override;
private:
	FTimerHandle ShotTimerHandle;
};
