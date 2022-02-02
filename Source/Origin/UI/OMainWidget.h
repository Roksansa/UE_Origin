// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OAmmoDescWidget.h"
#include "OTypes.h"
#include "Actors/Weapon/OBaseWeapon.h"
#include "Blueprint/UserWidget.h"
#include "OMainWidget.generated.h"

class UOCrossHairWidget;

UCLASS()
class ORIGIN_API UOMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	class UOPrimaryAttrWidget* GetAttrWidget(EOPrimaryAttr Type);
	FName GetAttrEventName(EOPrimaryAttr Type);

	UFUNCTION()
	void OnChangeAiming(bool bIsAiming);
	void OnNotifyChangeWeapon(EOEquippableItemType EquippableItem);
	void OnNotifyUpdatedAmmoWeapon(EOAmmoType Type, int CurrentCount, int MaxCount, bool Infinity);

	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void AnimDied();
	UFUNCTION()
	void OnHealthChanged(float CurrentValue, float Diff, float MaxValue);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetName")
	FName HealthWidgetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetName")
	FName StaminaWidgetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetName")
	FName AmmoDescWidgetName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="EventName")
	FName OnHealthChangedName = FName("OnPrimaryAttrChanged");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="EventName")
	FName OnStaminaChangedName = FName("OnPrimaryAttrChanged");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WidgetName")
	TMap<EOEquippableItemType, UOCrossHairWidget*> CrossHairWidgets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta=(BindWidgetAnim))
	UWidgetAnimation* HidePlayerShowSpec;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta=(BindWidgetAnim))
	UWidgetAnimation* ShowBlood;

private:
	UPROPERTY()
	UOCrossHairWidget* CurrentCrossHairWidget;

	UPROPERTY()
	UOAmmoDescWidget* AmmoDescWidget;
};
