// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTypes.h"
#include "GameFramework/Actor.h"
#include "OBaseWeapon.generated.h"

class USkeletalMeshComponent;

DECLARE_MULTICAST_DELEGATE(FOnMakeShot);
UCLASS(Abstract, NotBlueprintable)
class ORIGIN_API AOBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	AOBaseWeapon();

	virtual void StartFire(){}
	virtual void StopFire(){};

	/**Add check trace from muzzle - set on spawn weapon from weapon component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	bool bComplexTrace = false;

	EEquippableItemType GetItemType() const;
	
	FOnMakeShot OnMakeShot;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|WeaponParams")
	USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	float TraceMaxDistance = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|WeaponParams")
	EEquippableItemType EquippableItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|WeaponParams")
	float DamageAmount = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|WeaponParams")
	UAnimMontage* FireAnimMontage;
	
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
};
