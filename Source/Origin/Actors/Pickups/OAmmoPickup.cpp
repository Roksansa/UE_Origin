// Fill out your copyright notice in the Description page of Project Settings.


#include "OAmmoPickup.h"

#include "Characters/OBaseCharacter.h"

bool AOAmmoPickup::GivePickupTo(AOBaseCharacter* Character)
{
	return Character->TryAddAmmo(Type, Bullets);
}
