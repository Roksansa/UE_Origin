// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OSpectatorWidget.generated.h"

class UTextBlock;

UCLASS()
class ORIGIN_API UOSpectatorWidget : public UUserWidget
{
	GENERATED_BODY()
public:
void UpdateTimeRespawn(int32 LeftTimerInSec);
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta=(BindWidget))
	UTextBlock* RespawnText;
};
