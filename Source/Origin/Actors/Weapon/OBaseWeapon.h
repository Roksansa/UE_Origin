// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTypes.h"
#include "GameFramework/Actor.h"
#include "OBaseWeapon.generated.h"

class USkeletalMeshComponent;

UENUM(BlueprintType)
enum class EOAmmoType : uint8
{
	None,
	Rifle,
	Launcher,
	Shotgun,
	MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct ORIGIN_API FOAmmoData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AmmoParams")
	int32 BulletsInClip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AmmoParams")
	EOAmmoType AmmoType;
};

DECLARE_MULTICAST_DELEGATE(FOnMakeShot);
DECLARE_MULTICAST_DELEGATE(FOnStopFire);
DECLARE_MULTICAST_DELEGATE(FOnWasOutOfBullets);

UCLASS(Abstract, NotBlueprintable)
class ORIGIN_API AOBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	AOBaseWeapon();

	/**Add check trace from muzzle - set on spawn weapon from weapon component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	bool bComplexTrace = false;
	
	virtual void StartFire(){};
	virtual void StopFire();
	
	EOEquippableItemType GetItemType() const;
	FOnMakeShot OnMakeShot;
	FOnWasOutOfBullets OnWasOutOfBullets;
	FOnStopFire OnStopFire;
	bool IsAmmoEmpty() const;
	const FOAmmoData& GetAmmoData() const;
	int32 GetCountBullets() const;
	bool SetCountBullets(int32 NewCount);
	bool IsAutoReload() const;
	bool IsFullAuto() const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|WeaponParams")
	USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	float TraceMaxDistance = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|WeaponParams")
	EOEquippableItemType EquippableItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|WeaponParams")
	float DamageAmount = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|WeaponParams")
	UAnimMontage* FireAnimMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|WeaponParams")
	FOAmmoData DefaultAmmo = FOAmmoData{30, EOAmmoType::Rifle};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|WeaponParams")
	int32 CurrentBullets = 25;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|WeaponParams")
	bool bIsAutoReload = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|WeaponParams")
	bool bIsFullAuto = false;
	
	virtual void BeginPlay() override;

	virtual void MakeShot(){};
	virtual void MakeDamage(const FHitResult& HitResult) const;
	virtual FVector GetShootDirection(const FVector& ViewRotationVector) const;
	virtual void PlayVisibleShot();
	
	APlayerController* GetPlayerController() const;
	bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;
	FVector GetMuzzleWorldLocation() const;
	bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;
	void MakeHit(FHitResult& HitResult, const FVector TraceStart, const FVector TraceEnd) const;
	
	/** Play Animation Montage on the Skeletal mesh. Returns the length of the animation montage in seconds, or 0.f if failed to play. **/
	UFUNCTION(BlueprintCallable, Category = Animation)
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	void DecreaseAmmo();
};
