// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapon/OBaseWeapon.h"
#include "Blueprint/UserWidget.h"
#include "OAmmoDescWidget.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UOAmmoDescWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateAmmoWeapon(EOAmmoType Type, int CurrentCount, int MaxCount, bool Infinity);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Images")
	TMap<EOAmmoType, UTexture2D*> WeaponImages;
	EOAmmoType CurrentAmmoType = EOAmmoType::MAX;
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta=(BindWidget))
	class UTextBlock* CurrentAmmoText;
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta=(BindWidget))
	class UImage* CurrentAmmoImage;
};
