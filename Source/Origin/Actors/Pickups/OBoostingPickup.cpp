// Fill out your copyright notice in the Description page of Project Settings.


#include "OBoostingPickup.h"

#include "Characters/OBaseCharacter.h"

bool AOBoostingPickup::GivePickupTo(AOBaseCharacter* Character)
{
	return Character->TryAddBoosters(Type, Value);
}
