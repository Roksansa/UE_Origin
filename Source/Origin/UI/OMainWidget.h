// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTypes.h"
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

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetName")
	FName HealthWidgetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetName")
	FName StaminaWidgetName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="EventName")
	FName OnHealthChangedName = FName("OnPrimaryAttrChanged");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="EventName")
	FName OnStaminaChangedName = FName("OnPrimaryAttrChanged");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WidgetName")
	TMap<EOEquippableItemType, UOCrossHairWidget*> CrossHairWidgets;

private:
	UPROPERTY()
	UOCrossHairWidget* CurrentCrossHairWidget;
};
