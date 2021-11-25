// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTypes.h"
#include "Blueprint/UserWidget.h"
#include "OPrimaryAttrWidget.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UOPrimaryAttrWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void OnPrimaryAttrChanged(float CurrentValue, float Diff, float MaxValue);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config")
	EOPrimaryAttr Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config")
	FLinearColor MaxPercentColor;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config")
	FLinearColor MinPercentColor;

	UPROPERTY(BlueprintReadOnly, Category = "Config", meta=(BindWidget))
	class UProgressBar* CurrentProgressBar;

	UPROPERTY(BlueprintReadOnly, Category = "Config", meta=(BindWidget))
	class UTextBlock* CurrentText;
};
