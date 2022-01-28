// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Pickups/OBasePickup.h"
#include "Actors/Weapon/OBaseWeapon.h"
#include "OAmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API AOAmmoPickup : public AOBasePickup
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	EOAmmoType Type;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting", meta =(ClampMin = "0", UIMin = "0"))
	int32 Bullets;
private:
	virtual bool GivePickupTo(AOBaseCharacter* Character) override;
};
