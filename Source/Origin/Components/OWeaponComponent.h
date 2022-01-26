// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTypes.h"
#include "Actors/Weapon/OBaseWeapon.h"
#include "Components/ActorComponent.h"
#include "OWeaponComponent.generated.h"


USTRUCT(BlueprintType)
struct ORIGIN_API FOWeaponAnimDescription
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Anim")
	UAnimMontage* HitAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Anim")
	UAnimMontage* HipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Anim")
	UAnimMontage* ReloadAnimMontage;
};

USTRUCT(BlueprintType)
struct ORIGIN_API FOAmmoDescription
{
	GENERATED_BODY()

	/**
	 * In AmmoDescriptions - max count for this ammo type
	 * in CurrentAmmo - current Bullets count for this character
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Ammo", meta =(EditCondition ="!Infinity", ClampMin = 1, UIMin = 1))
	int32 BulletsCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Ammo")
	bool Infinity;
};

UENUM(BlueprintType)
enum class EOWeaponUseState : uint8
{
	None,
	Idle,
	Fire,
	Reload,
	Equip,
	MAX UMETA(Hidden)
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnNotifyFinishEquip, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnNotifyFinishReload, int32, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNotifyChangeWeapon, EOEquippableItemType);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnNotifyUpdatedAmmoWeapon, EOAmmoType, int, int, bool);

class AOBaseWeapon;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ORIGIN_API UOWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOWeaponComponent();

	void StartFire();
	void StopFire();

	const FOnNotifyFinishEquip& GetOnNotifyFinishEquip() const;
	const FOnNotifyFinishReload& GetOnNotifyFinishReload() const;
	
	FOWeaponAnimDescription GetWeaponAnimDescription() const;
	EOEquippableItemType GetWeaponType() const;
	int GetWeaponIndex() const;
	bool IsPlayingEquipMontage();
	/**
	 * WeaponNumber - correct start number 1.
	 * if  weaponNumber = 1 than weapon index = 0
	 */
	void NextWeapon(int WeaponNumber = 0);
	
	bool AddAmmo(const EOAmmoType& Type, int32 Count);
	void ReloadAmmo();
	

	EOWeaponUseState GetState() const;
	bool GetAiming() const;
	UFUNCTION(BlueprintCallable)
	float GetCurrentAimingFOV() const;

	FOnNotifyChangeWeapon OnNotifyChangeWeapon;
	FOnNotifyUpdatedAmmoWeapon OnNotifyUpdatedAmmoWeapon;

	void OnUpdateAmmo();
protected:

	FOnNotifyFinishEquip OnNotifyFinishEquip;
	FOnNotifyFinishReload OnNotifyFinishReload;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Weapon")
	TArray<TSubclassOf<AOBaseWeapon>> WeaponClasses;
	
	/**Add check trace from muzzle - set on spawn weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Weapon")
	bool bComplexTrace = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Weapon|Anim")
	UAnimMontage* EquipAnimMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config|Weapon")
	TMap<EOEquippableItemType, FOWeaponAnimDescription> WeaponAnimDescriptions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config|Weapon")
	TMap<EOAmmoType, FOAmmoDescription> AmmoDescriptions;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	EOWeaponUseState State = EOWeaponUseState::Idle;
	
	UPROPERTY()
	AOBaseWeapon* CurrentWeapon;
	UPROPERTY()
	TArray<AOBaseWeapon*> ArmoryWeapons;
	
	static constexpr int AmmoStart = 1;
	static constexpr int AmmoCount = static_cast<int>(EOAmmoType::MAX) - AmmoStart;

	TArray<FOAmmoDescription, TInlineAllocator<AmmoCount>> CurrentAmmo;
	FTimerHandle ReloadTimerHandle;
	int CurrentIndex = 0;

	bool bAiming = false;
	
	void InitAmmo();
	void SpawnWeapons();
	void InitAnimation();
	
	void AttachWeaponToSocket(AOBaseWeapon* Weapon, USkeletalMeshComponent* Mesh, const FName& EquipSocketName);
	bool CanFire() const;
	void EquipWeapon(int WeaponIndex);
	bool CanReload() const;
	int GetAmmoIndex(EOAmmoType AmmoType) const;
	UFUNCTION()
	void OnFinishReload(int32 ReloadCount, bool bUseReloadCount);
	void EndReload();
	void Reload();

	UFUNCTION()
	void AmmoDesc();
	
	UFUNCTION()
	void EndFire() const;
	UFUNCTION()
	void OnFinishEquip(bool bIsOldWeapon);
	float PlayAnimMontage(UAnimMontage* AnimMontage) const;

	UFUNCTION()
	void ResetFireInWeapon();

	UFUNCTION()
	void OnAllowFire(bool bAllowFire);
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	void AddEventForMontageEnded();
	void RemoveEventForMontageEnded();

	UFUNCTION()
	void ChangeAiming(bool bWantAiming);
};
