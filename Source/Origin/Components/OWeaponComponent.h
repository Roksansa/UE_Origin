// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OWeaponComponent.generated.h"

class AOBaseWeapon;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ORIGIN_API UOWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOWeaponComponent();

	void Fire();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Weapon")
	TSubclassOf<AOBaseWeapon> WeaponClass;

	/**Add check trace from muzzle - set on spawn weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Weapon")
	bool bComplexTrace = false;
private:
	UPROPERTY()
	AOBaseWeapon* CurrentWeapon;
	
	void SpawnWeapon();
};
