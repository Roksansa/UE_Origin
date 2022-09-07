// Fill out your copyright notice in the Description page of Project Settings.


#include "OPlayerStatsWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"

void UOPlayerStatsWidget::UpdateRounds(int32 Current, int32 Max)
{
	FString Str = FString::Format(TEXT("{0} / {1}"), {Current, Max});
	RoundsText->SetText(UKismetTextLibrary::Conv_StringToText(Str));
}

void UOPlayerStatsWidget::UpdateTimer(int32 LeftTimerInSec)
{
	int32 Minutes = LeftTimerInSec / 60;
	int32 Sec = LeftTimerInSec % 60;
	FString Str = FString::Format(TEXT("{0}:{1}"), {Minutes, Sec});
	TimerText->SetText(UKismetTextLibrary::Conv_StringToText(Str));
}

void UOPlayerStatsWidget::UpdatePlayerStats(int32 Kills, int32 Deaths)
{
	KillsText->SetText(UKismetTextLibrary::Conv_StringToText(FString::FromInt(Kills)));
	DeathsText->SetText(UKismetTextLibrary::Conv_StringToText(FString::FromInt(Deaths)));
}
