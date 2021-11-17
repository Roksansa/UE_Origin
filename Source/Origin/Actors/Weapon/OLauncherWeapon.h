// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapon/OBaseWeapon.h"
#include "OLauncherWeapon.generated.h"

class AOProjectile;

UCLASS(Blueprintable)
class ORIGIN_API AOLauncherWeapon : public AOBaseWeapon
{
	GENERATED_BODY()

public:
	virtual void StartFire() override;
	virtual void StopFire() override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components|WeaponParams")
	TSubclassOf<AOProjectile> ProjectileType;
	
	virtual void MakeShot() override;
};
