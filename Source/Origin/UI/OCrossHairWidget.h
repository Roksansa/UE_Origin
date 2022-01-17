// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OCrossHairWidget.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UOCrossHairWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta=(BindWidgetAnim))
	UWidgetAnimation* Aiming;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta=(BindWidgetAnim))
	UWidgetAnimation* Hide;
public:
	UFUNCTION()
	void SwitchAnimAim(bool bIsAiming);
	UFUNCTION()
	void SwitchAnimHide(bool bIsVisibility);
};
