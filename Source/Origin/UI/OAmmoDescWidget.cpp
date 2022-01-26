// Fill out your copyright notice in the Description page of Project Settings.


#include "OAmmoDescWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"

void UOAmmoDescWidget::UpdateAmmoWeapon(EOAmmoType Type, int CurrentCount, int MaxCount, bool Infinity)
{
	if (Type != CurrentAmmoType)
	{
		CurrentAmmoType = Type;
		if (Type == EOAmmoType::None)
		{
			SetVisibility(ESlateVisibility::Hidden);
			return;
		}
		SetVisibility(ESlateVisibility::Visible);
		CurrentAmmoImage->SetBrushFromTexture(WeaponImages[Type], false);
	}
	if (Type != EOAmmoType::None)
	{
		const FString InfinityText = TEXT("\U0000221E");
		const FString MaxCountText = FString::FromInt(MaxCount);
		const FString Str = FString::Printf(TEXT("%i/%ws"), CurrentCount, Infinity ? *InfinityText : *MaxCountText);
		CurrentAmmoText->SetText(UKismetTextLibrary::Conv_StringToText(Str));
	}
}
