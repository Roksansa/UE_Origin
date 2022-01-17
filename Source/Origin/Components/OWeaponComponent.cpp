// Fill out your copyright notice in the Description page of Project Settings.


#include "OWeaponComponent.h"

#include "OTypes.h"
#include "Actors/Weapon/OBaseWeapon.h"
#include "Characters/OBaseCharacter.h"
#include "Characters/OPlayerCharacter.h"
#include "Characters/Controllers/OPlayerController.h"
#include "GameFramework/Character.h"


DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All);

UOWeaponComponent::UOWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UOWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapons();
	InitAnimation();

	InitAmmo();
	OnNotifyFinishEquip.AddUObject(this, &UOWeaponComponent::OnFinishEquip);
	OnNotifyFinishReload.AddUObject(this,&UOWeaponComponent::OnFinishReload);
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

bool UOWeaponComponent::GetAiming() const
{
	return bAiming;
}

float UOWeaponComponent::GetCurrentAimingFOV() const
{
	return CurrentWeapon ? CurrentWeapon->GetAimFOV() : 0.f;
}

void UOWeaponComponent::ChangeAiming(bool bWantAiming)
{
	bAiming = bWantAiming;
	//doto func
}

void UOWeaponComponent::AttachWeaponToSocket(AOBaseWeapon* Weapon, USkeletalMeshComponent* Mesh, const FName& EquipSocketName)
{
	if (!Weapon)
	{
		return;
	}
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

	if (AOBaseCharacter* BaseCharacter = Cast<AOBaseCharacter>(GetOwner()))
	{
		BaseCharacter->OnChangeAiming.AddDynamic(this, &UOWeaponComponent::ChangeAiming);
		
		if (AOPlayerCharacter* PlayerCharacter = Cast<AOPlayerCharacter>(GetOwner()))
		{
			PlayerCharacter->OnAllowFire.AddUObject(this, &UOWeaponComponent::OnAllowFire);
		}
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
			Weapon->OnWasOutOfBullets.AddUObject(this, &UOWeaponComponent::Reload);
			Weapon->OnMakeShot.AddUObject(this, &UOWeaponComponent::AmmoDesc);
			Weapon->OnStopFire.AddUObject(this, &UOWeaponComponent::ResetFireInWeapon);
		}
	}

	State = EOWeaponUseState::Idle;
	EquipWeapon(0);
}

bool UOWeaponComponent::CanFire() const
{
	return CurrentWeapon && State == EOWeaponUseState::Idle && !CurrentWeapon->IsAmmoEmpty();
}

void UOWeaponComponent::StartFire()
{
	if (CanFire())
	{
		State = EOWeaponUseState::Fire;
		CurrentWeapon->StartFire();
		return;
	}
	if (CurrentWeapon && CurrentWeapon->IsAmmoEmpty() && CurrentWeapon->IsAutoReload())
	{
		Reload();
	}
}

void UOWeaponComponent::StopFire()
{
	if (!CurrentWeapon)
	{
		return;
	}
	if (State == EOWeaponUseState::Fire)
	{
		State = EOWeaponUseState::Idle;
	}
	CurrentWeapon->StopFire();
}

const FOnNotifyFinishEquip& UOWeaponComponent::GetOnNotifyFinishEquip() const
{
	return OnNotifyFinishEquip;
}

const FOnNotifyFinishReload& UOWeaponComponent::GetOnNotifyFinishReload() const
{
	return OnNotifyFinishReload;
}

FOWeaponAnimDescription UOWeaponComponent::GetWeaponAnimDescription() const
{
	if (CurrentWeapon)
	{
		return WeaponAnimDescriptions.Contains(CurrentWeapon->GetItemType())
			       ? WeaponAnimDescriptions[CurrentWeapon->GetItemType()]
			       : FOWeaponAnimDescription{};
	}
	return FOWeaponAnimDescription{};
}

EOEquippableItemType UOWeaponComponent::GetWeaponType() const
{
	return CurrentWeapon ? CurrentWeapon->GetItemType() : EOEquippableItemType::None;
}

int UOWeaponComponent::GetWeaponIndex() const
{
	int CurIndex = ArmoryWeapons.Find(CurrentWeapon);

	CurIndex++;
	if (CurIndex >= ArmoryWeapons.Num())
	{
		CurIndex = 0;
	}
	return CurIndex;
}

bool UOWeaponComponent::IsPlayingEquipMontage()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		UAnimInstance * AnimInstance = (Character->GetMesh())? Character->GetMesh()->GetAnimInstance() : nullptr;
		if (AnimInstance)
		{
			TArray<struct FAnimMontageInstance*> Anims = AnimInstance->MontageInstances;
			for (auto Anim : Anims)
			{
				if (Anim->Montage == EquipAnimMontage)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void UOWeaponComponent::NextWeapon(int WeaponNumber)
{
	if (CurrentIndex == (WeaponNumber-1) || State != EOWeaponUseState::Idle && State != EOWeaponUseState::Fire)
	{
		return;
	}

	if (IsPlayingEquipMontage()) return;
	
	CurrentIndex = GetWeaponIndex();

	if (WeaponNumber > 0 && WeaponNumber <= ArmoryWeapons.Num())
	{
		CurrentIndex = WeaponNumber - 1;
	}
	EquipWeapon(CurrentIndex);
}

bool UOWeaponComponent::AddAmmo(const EOAmmoType& Type, int32 Count)
{
	if (Count <= 0 || !AmmoDescriptions.Contains(Type) || AmmoDescriptions[Type].Infinity)
	{
		return false;
	}
	const int AmmoIndex = GetAmmoIndex(Type);
	const int32 CurrentBullets = CurrentAmmo[AmmoIndex].BulletsCount;
	const int32 BulletsCountMax = AmmoDescriptions[Type].BulletsCount;
	//Full ammo
	if (CurrentBullets >= BulletsCountMax)
	{
		return false;
	}

	CurrentAmmo[AmmoIndex].BulletsCount = FMath::Clamp(CurrentAmmo[AmmoIndex].BulletsCount + Count, 0, BulletsCountMax);
	return true;
}

void UOWeaponComponent::ReloadAmmo()
{
	Reload();
}

EOWeaponUseState UOWeaponComponent::GetState() const
{
	return State;
}

void UOWeaponComponent::InitAmmo()
{
	CurrentAmmo.Reset(AmmoCount);
	const auto AmmoEnum = StaticEnum<EOAmmoType>();
	check(AmmoEnum);
	for (int32 i = AmmoStart; i < AmmoEnum->NumEnums() - 1; i++)
	{
		const EOAmmoType EnumType = static_cast<EOAmmoType>(i);
		const FString EnumTypeName = UEnum::GetValueAsString(EnumType);
		const bool AmmoCheckCond = AmmoDescriptions.Contains(EnumType);
		checkf(AmmoCheckCond, TEXT("AmmoDesc for %s doesn't exist"), *EnumTypeName);
		CurrentAmmo.Add(FOAmmoDescription{0, AmmoDescriptions[EnumType].Infinity});
		CurrentAmmo[i-AmmoStart].BulletsCount = 10;
	}
}

void UOWeaponComponent::EquipWeapon(int WeaponIndex)
{
	StopFire();
	State = EOWeaponUseState::Equip;
	if (CurrentWeapon || ArmoryWeapons[WeaponIndex])
	{
		const float Duration = PlayAnimMontage(EquipAnimMontage);
		if (Duration > 0.f)
		{
			AddEventForMontageEnded();
		}
	}
	else
	{
		State = EOWeaponUseState::Idle;
		OnNotifyChangeWeapon.Broadcast(GetWeaponType());
	}
}

bool UOWeaponComponent::CanReload() const
{
	const bool MainCondition = (State == EOWeaponUseState::Idle || State == EOWeaponUseState::Fire) && CurrentWeapon;
	if (MainCondition)
	{
		const int32 AmmoIndex = GetAmmoIndex(CurrentWeapon->GetAmmoData().AmmoType);
		return CurrentAmmo[AmmoIndex].Infinity || CurrentAmmo[AmmoIndex].BulletsCount > 0;
	}

	return false;
}

int UOWeaponComponent::GetAmmoIndex(EOAmmoType AmmoType) const
{
	return static_cast<int>(AmmoType) - AmmoStart;
}

void UOWeaponComponent::OnFinishReload(int32 ReloadCount, bool bUseReloadCount)
{
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	const EOAmmoType AmmoType = CurrentWeapon->GetAmmoData().AmmoType;
	const int AmmoIndex = GetAmmoIndex(AmmoType);
	const int32 CurrentBullets = !CurrentAmmo[AmmoIndex].Infinity
		                             ? CurrentAmmo[AmmoIndex].BulletsCount
		                             : AmmoDescriptions[AmmoType].BulletsCount;
	const int32 BulletsInClip = CurrentWeapon->GetAmmoData().BulletsInClip;
	
	int32 NeedBulletsCount = BulletsInClip - CurrentWeapon->GetCountBullets();
	NeedBulletsCount = bUseReloadCount ? FMath::Min(ReloadCount,NeedBulletsCount) : NeedBulletsCount;
	
	const int32 AddCount = FMath::Min(NeedBulletsCount, CurrentBullets);
	UE_LOG(LogWeaponComponent, Display, TEXT("Reload %i --- %i --- %i...."), CurrentBullets, NeedBulletsCount, AddCount);
	if (!CurrentAmmo[AmmoIndex].Infinity)
	{
		CurrentAmmo[AmmoIndex].BulletsCount -= AddCount;
	}
	CurrentWeapon->SetCountBullets(AddCount);
	AmmoDesc();

	if (!bUseReloadCount)
	{
		EndReload();
		return;
	}

	const bool bNeedRepeatReload = CurrentAmmo[AmmoIndex].BulletsCount > 0 && BulletsInClip - CurrentWeapon->GetCountBullets() > 0;

	if (bNeedRepeatReload)
	{
		ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
		UAnimInstance* AnimInstance = CharacterOwner ? CharacterOwner->GetMesh()->GetAnimInstance() : nullptr;
		if (IsValid(AnimInstance))
		{
			AnimInstance->Montage_JumpToSection(SectionMontageStartBulletReload,
				WeaponAnimDescriptions[CurrentWeapon->GetItemType()].ReloadAnimMontage);
			UE_LOG(LogWeaponComponent, Display, TEXT("Repeated reload...."));
			return;
		}
	}
	
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, [this]()
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
		EndReload();
	}, 0.2f, false);
}

void UOWeaponComponent::EndReload()
{
	State = EOWeaponUseState::Idle;
	if (CurrentWeapon && CurrentWeapon->IsAutoReload())
	{
		const ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
		const AOPlayerController* Controller = IsValid(CharacterOwner) ? Cast<AOPlayerController>(CharacterOwner->GetController()) : nullptr;
		if (Controller && Controller->IsPressedAnyKeyForAction(WeaponFireActionName))
		{
			StartFire();
		}
	}
	UE_LOG(LogWeaponComponent, Display, TEXT("End reload...."));
}

void UOWeaponComponent::Reload()
{
	if (!CanReload())
	{
		return;
	}
	UE_LOG(LogWeaponComponent, Display, TEXT("Start reload...."));
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	State = EOWeaponUseState::Reload;
	const float Duration = PlayAnimMontage(WeaponAnimDescriptions[CurrentWeapon->GetItemType()].ReloadAnimMontage);
	if (Duration < 0.1)
	{
		OnFinishReload(-1, false);
	}
	else
	{
		AddEventForMontageEnded();
		GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, [this]() { OnFinishReload(-1, false); }, Duration, false);
	}
}

void UOWeaponComponent::AmmoDesc()
{
	EOAmmoType AmmoType = CurrentWeapon->GetAmmoData().AmmoType;
	const int AmmoIndex = GetAmmoIndex(AmmoType);
	const int32 CurrentBullets = CurrentAmmo[AmmoIndex].BulletsCount;
	UE_LOG(LogWeaponComponent, Display, TEXT("Ammo weapon %i / %i, max ammo %i ,Infinity %i"), CurrentWeapon->GetCountBullets(),
		CurrentBullets, AmmoDescriptions[AmmoType].BulletsCount, AmmoDescriptions[AmmoType].Infinity);
}

void UOWeaponComponent::EndFire() const
{
	const FOWeaponAnimDescription Desc = GetWeaponAnimDescription();
	PlayAnimMontage(Desc.HipAnimMontage);
}

float UOWeaponComponent::PlayAnimMontage(UAnimMontage* AnimMontage) const
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		return Character->PlayAnimMontage(AnimMontage);
	}
	return 0.f;
}

void UOWeaponComponent::InitAnimation()
{
}

void UOWeaponComponent::OnFinishEquip(bool bIsOldWeapon)
{
	if (!bIsOldWeapon)
	{
		State = EOWeaponUseState::Idle;
		return;
	}
	const ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		return;
	}
	AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), ArmoryEquipSocketName);
	CurrentWeapon = ArmoryWeapons[CurrentIndex];
	AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponSocketName);
	OnNotifyChangeWeapon.Broadcast(GetWeaponType());
}

void UOWeaponComponent::ResetFireInWeapon()
{
	if (State == EOWeaponUseState::Fire)
	{
		State = EOWeaponUseState::Idle;
	}
}

void UOWeaponComponent::OnAllowFire(bool bAllowFire)
{
	if (bAllowFire && State == EOWeaponUseState::Idle && CurrentWeapon && CurrentWeapon->IsFullAuto())
	{
		StartFire();
	}
	if (!bAllowFire && State == EOWeaponUseState::Fire && CurrentWeapon && CurrentWeapon->IsFullAuto())
	{
		StopFire();
	}
}

void UOWeaponComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == EquipAnimMontage)
	{
		if (bInterrupted)
		{
			CurrentIndex = GetWeaponIndex();
			if (State == EOWeaponUseState::Equip)
			{
				OnNotifyChangeWeapon.Broadcast(GetWeaponType());
				State = EOWeaponUseState::Idle;
			}
		}
		RemoveEventForMontageEnded();
	}
	if (Montage == GetWeaponAnimDescription().ReloadAnimMontage)
	{
		if (bInterrupted)
		{
			if (State == EOWeaponUseState::Reload)
			{
				State = EOWeaponUseState::Idle;
			}
			GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
		}
		RemoveEventForMontageEnded();
	}
}

void UOWeaponComponent::AddEventForMontageEnded()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		UAnimInstance * AnimInstance = (Character->GetMesh())? Character->GetMesh()->GetAnimInstance() : nullptr; 
		if(AnimInstance)
		{
			AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &UOWeaponComponent::OnMontageEnded);
		}
	}
}

void UOWeaponComponent::RemoveEventForMontageEnded()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		UAnimInstance * AnimInstance = (Character->GetMesh())? Character->GetMesh()->GetAnimInstance() : nullptr; 
		if(AnimInstance)
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &UOWeaponComponent::OnMontageEnded);
		}
	}
}
