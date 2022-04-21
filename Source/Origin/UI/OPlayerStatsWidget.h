// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OPlayerStatsWidget.generated.h"

class UTextBlock;

UCLASS()
class ORIGIN_API UOPlayerStatsWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateRounds(int32 Current, int32 Max);
	void UpdateTimer(int32 LeftTimerInSec);
	void UpdatePlayerStats(int32 Kills, int32 Deaths);
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta=(BindWidget))
	UTextBlock* RoundsText;
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta=(BindWidget))
	UTextBlock* TimerText;
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta=(BindWidget))
	UTextBlock* KillsText;
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta=(BindWidget))
	UTextBlock* DeathsText;
};
