// Fill out your copyright notice in the Description page of Project Settings.


#include "OAICharacter.h"

AOAICharacter::AOAICharacter(const FObjectInitializer& ObjectInitializer)
{
}

void AOAICharacter::BeginPlay()
{
	Super::BeginPlay();
	NextWeaponIndex(2);
}

void AOAICharacter::StartFire()
{
	TryChangeOrReload(true);
	WeaponComponent->StartFire();
}

void AOAICharacter::StopFire()
{
	TryChangeOrReload();
}

void AOAICharacter::ReloadAmmo()
{
	TryChangeOrReload();
}

void AOAICharacter::NextWeapon()
{
	TryChangeOrReload();
}

bool AOAICharacter::TryChangeOrReload(bool bCheckAmmoEmpty)
{
	if (!CanUseWeapon() || !WeaponComponent->CanAmmoAnyWeapon())
	{
		return false;
	}
	if (!WeaponComponent->GetWeapon() || (!WeaponComponent->CanReload() && WeaponComponent->GetWeapon()->IsAmmoEmpty()))
	{
		WeaponComponent->NextWeapon();
		return true;
	}
	if (WeaponComponent->CanReload())
	{
		if (!bCheckAmmoEmpty || WeaponComponent->GetWeapon()->IsAmmoEmpty())
		{
			WeaponComponent->ReloadAmmo();
		}
	}
	return false;
}