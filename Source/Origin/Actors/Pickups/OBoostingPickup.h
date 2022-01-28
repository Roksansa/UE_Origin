// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTypes.h"
#include "Actors/Pickups/OBasePickup.h"
#include "OBoostingPickup.generated.h"


UCLASS()
class ORIGIN_API AOBoostingPickup : public AOBasePickup
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	EOBoostingType Type;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting", meta =(ClampMin = "0", UIMin = "0"))
	int32 Value;
private:
	virtual bool GivePickupTo(AOBaseCharacter* Character) override;
};
