// Fill out your copyright notice in the Description page of Project Settings.


#include "OPrimaryAttrWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"

void UOPrimaryAttrWidget::OnPrimaryAttrChanged(float CurrentValue, float Diff, float MaxValue)
{
	float Percent = CurrentValue/MaxValue;
	CurrentProgressBar->SetPercent(Percent);
	FLinearColor Color = FMath::Lerp(MinPercentColor, MaxPercentColor, Percent);
	CurrentProgressBar->SetFillColorAndOpacity(Color);
	const FString Str = FString::Printf(TEXT("%.0f/%.0f"), CurrentValue,MaxValue);
	CurrentText->SetText(UKismetTextLibrary::Conv_StringToText(Str));
}
