// Fill out your copyright notice in the Description page of Project Settings.


#include "OWeaponComponent.h"

#include "OTypes.h"
#include "Actors/Weapon/OBaseWeapon.h"
#include "GameFramework/Character.h"

UOWeaponComponent::UOWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UOWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
}

void UOWeaponComponent::SpawnWeapon()
{
	if (!GetWorld())
	{
		return;
	}
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		return;
	}
	CurrentWeapon = GetWorld()->SpawnActor<AOBaseWeapon>(WeaponClass);

	if (!CurrentWeapon)
	{
		return;
	}

	const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	CurrentWeapon->AttachToComponent(Character->GetMesh(), AttachmentTransformRules, WeaponSocketName);
	CurrentWeapon->SetOwner(GetOwner());
	CurrentWeapon->SetInstigator(Character);
	CurrentWeapon->bComplexTrace = bComplexTrace;
}


void UOWeaponComponent::Fire()
{
	if (!CurrentWeapon)
	{
		return;
	}
	CurrentWeapon->Fire();
}
