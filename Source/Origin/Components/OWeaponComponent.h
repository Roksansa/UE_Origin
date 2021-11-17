// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTypes.h"
#include "Components/ActorComponent.h"
#include "OWeaponComponent.generated.h"


USTRUCT(BlueprintType)
struct FOWeaponAnimDescription
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Anim")
	UAnimMontage* HitAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Anim")
	UAnimMontage* HipAnimMontage;
};

UENUM(BlueprintType)
enum class EWeaponUseState : uint8
{
	None,
	Idle,
	Reload,
	Equip,
	MAX UMETA(Hidden)
};

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

	FOWeaponAnimDescription GetWeaponAnimDescription() const;
	EEquippableItemType GetWeaponType() const;
	void NextWeapon();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Weapon")
	TArray<TSubclassOf<AOBaseWeapon>> WeaponClasses;
	
	/**Add check trace from muzzle - set on spawn weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Weapon")
	bool bComplexTrace = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Weapon|Anim")
	UAnimMontage* EquipAnimMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config|Weapon")
	TMap<EEquippableItemType, FOWeaponAnimDescription> WeaponAnimDescriptions;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	UPROPERTY()
	AOBaseWeapon* CurrentWeapon;

	UPROPERTY()
	TArray<AOBaseWeapon*> ArmoryWeapons;

	EWeaponUseState State = EWeaponUseState::Idle;

	void AttachWeaponToSocket(AOBaseWeapon* Weapon, USkeletalMeshComponent* Mesh, const FName& EquipSocketName);
	void SpawnWeapons();
	void EquipWeapon(int WeaponIndex);
	
	UFUNCTION()
	void EndFire() const;
	void PlayAnimMontage(UAnimMontage* AnimMontage) const;
};
