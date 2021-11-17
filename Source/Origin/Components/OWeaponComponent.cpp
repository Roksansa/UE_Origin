// Fill out your copyright notice in the Description page of Project Settings.


#include "OWeaponComponent.h"

#include "OTypes.h"
#include "Actors/Weapon/OBaseWeapon.h"
#include "Characters/OBaseCharacter.h"
#include "GameFramework/Character.h"

UOWeaponComponent::UOWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UOWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapons();
}

void UOWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CurrentWeapon = nullptr;
	for (auto Weapon : ArmoryWeapons)
	{
		if (Weapon != nullptr)
		{
			Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			Weapon->Destroy();
		}
	}
	ArmoryWeapons.Empty();
	
	Super::EndPlay(EndPlayReason);
}

void UOWeaponComponent::AttachWeaponToSocket(AOBaseWeapon* Weapon, USkeletalMeshComponent* Mesh, const FName& EquipSocketName)
{
	const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	Weapon->AttachToComponent(Mesh, AttachmentTransformRules, EquipSocketName);
}

void UOWeaponComponent::SpawnWeapons()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!GetWorld() || !Character)
	{
		return;
	}

	ArmoryWeapons.Reset(0);
	ArmoryWeapons.Add(nullptr);

	for (auto WeaponClass : WeaponClasses)
	{
		AOBaseWeapon* Weapon = GetWorld()->SpawnActor<AOBaseWeapon>(WeaponClass);
		if (Weapon)
		{
			ArmoryWeapons.Add(Weapon);
			AttachWeaponToSocket(Weapon, Character->GetMesh(), ArmoryEquipSocketName);
			Weapon->SetOwner(GetOwner());
			Weapon->SetInstigator(Character);
			Weapon->bComplexTrace = bComplexTrace;
			Weapon->OnMakeShot.AddUObject(this, &UOWeaponComponent::EndFire);
		}
	}

	State = EWeaponUseState::Idle;
	EquipWeapon(0);
	

	// if (!CurrentWeapon)
	// {
	// 	return;
	// }

	// const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	// CurrentWeapon->AttachToComponent(Character->GetMesh(), AttachmentTransformRules, WeaponSocketName);
	// CurrentWeapon->SetOwner(GetOwner());
	// CurrentWeapon->SetInstigator(Character);
	// CurrentWeapon->bComplexTrace = bComplexTrace;
	// CurrentWeapon->OnMakeShot.AddUObject(this, &UOWeaponComponent::EndFire);

}

void UOWeaponComponent::StartFire()
{
	if (!CurrentWeapon || State != EWeaponUseState::Idle)
	{
		return;
	}
	CurrentWeapon->StartFire();
}

void UOWeaponComponent::StopFire()
{
	if (!CurrentWeapon)
	{
		return;
	}
	CurrentWeapon->StopFire();
}

FOWeaponAnimDescription UOWeaponComponent::GetWeaponAnimDescription() const
{
	if (CurrentWeapon)
	{
		return WeaponAnimDescriptions.Contains(CurrentWeapon->GetItemType()) ? WeaponAnimDescriptions[CurrentWeapon->GetItemType()] : FOWeaponAnimDescription{};
	}
	return FOWeaponAnimDescription{};
}

EEquippableItemType UOWeaponComponent::GetWeaponType() const
{
	return CurrentWeapon ? CurrentWeapon->GetItemType() : EEquippableItemType::None;
}

void UOWeaponComponent::NextWeapon()
{
	int CurIndex = ArmoryWeapons.Find(CurrentWeapon);
	
	CurIndex++;
	if (CurIndex >= ArmoryWeapons.Num())
	{
		CurIndex = 0;
	}
	EquipWeapon(CurIndex);
}

void UOWeaponComponent::EquipWeapon(int WeaponIndex)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	State = EWeaponUseState::Equip;
	PlayAnimMontage(EquipAnimMontage);
	if (CurrentWeapon)
	{
		StopFire();
		AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), ArmoryEquipSocketName);
	}
	CurrentWeapon = ArmoryWeapons[WeaponIndex];
	if (CurrentWeapon)
	{
		AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponSocketName);
	}
}

void UOWeaponComponent::EndFire() const
{
	const FOWeaponAnimDescription Desc = GetWeaponAnimDescription();
	PlayAnimMontage(Desc.HipAnimMontage);
}

void UOWeaponComponent::PlayAnimMontage(UAnimMontage* AnimMontage) const
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		Character->PlayAnimMontage(AnimMontage);
	}
}
