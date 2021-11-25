// Fill out your copyright notice in the Description page of Project Settings.


#include "OMainWidget.h"

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
