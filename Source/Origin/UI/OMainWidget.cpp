// Fill out your copyright notice in the Description page of Project Settings.


#include "OMainWidget.h"

#include "OCrossHairWidget.h"
#include "OPrimaryAttrWidget.h"
#include "Blueprint/WidgetTree.h"

UOPrimaryAttrWidget* UOMainWidget::GetAttrWidget(EOPrimaryAttr Type)
{
	FName WidgetName = Type == EOPrimaryAttr::Health ? HealthWidgetName : StaminaWidgetName;
	return WidgetTree->FindWidget<UOPrimaryAttrWidget>(WidgetName);
}

FName UOMainWidget::GetAttrEventName(EOPrimaryAttr Type)
{
	return Type == EOPrimaryAttr::Health ? OnHealthChangedName : OnStaminaChangedName;
}

void UOMainWidget::OnChangeAiming(bool bIsAiming)
{
	if (CurrentCrossHairWidget)
	{
		CurrentCrossHairWidget->SwitchAnimAim(bIsAiming);
	}
}

void UOMainWidget::OnNotifyChangeWeapon(EOEquippableItemType EquippableItem)
{
	if (CrossHairWidgets.Contains(EquippableItem) && CurrentCrossHairWidget != CrossHairWidgets[EquippableItem])
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, "1");
		if (CurrentCrossHairWidget)
		{
			CurrentCrossHairWidget->SwitchAnimHide(false);
		}
		
		CurrentCrossHairWidget = CrossHairWidgets[EquippableItem];
		
		if (CurrentCrossHairWidget)
		{
			CurrentCrossHairWidget->SwitchAnimHide(true);
		}
	}
}
