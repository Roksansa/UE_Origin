// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OPrimaryAttrWidget.h"
#include "OTypes.h"
#include "Blueprint/UserWidget.h"
#include "OMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UOMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UOPrimaryAttrWidget* GetAttrWidget(EOPrimaryAttr Type);

	FName GetAttrEventName(EOPrimaryAttr Type);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetName")
	FName HealthWidgetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetName")
	FName StaminaWidgetName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="EventName")
	FName OnHealthChangedName = FName("OnPrimaryAttrChanged");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="EventName")
	FName OnStaminaChangedName = FName("OnPrimaryAttrChanged");
};
