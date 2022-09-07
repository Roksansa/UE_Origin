// Fill out your copyright notice in the Description page of Project Settings.


#include "OSpectatorWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"

void UOSpectatorWidget::UpdateTimeRespawn(int32 LeftTimerInSec)
{
	int32 Minutes = LeftTimerInSec / 60;
	int32 Sec = LeftTimerInSec % 60;
	FString Str = FString::Format(TEXT("{0}:{1}"), {Minutes, Sec});
	RespawnText->SetText(UKismetTextLibrary::Conv_StringToText(Str));
}
