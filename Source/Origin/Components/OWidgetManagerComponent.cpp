// Fill out your copyright notice in the Description page of Project Settings.


#include "OWidgetManagerComponent.h"

#include "Characters/OBaseCharacter.h"
#include "UI/OMainWidget.h"
#include "UI/OPrimaryAttrWidget.h"

UOWidgetManagerComponent::UOWidgetManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOWidgetManagerComponent::BindWidgets(AOBaseCharacter* Character)
{
	if (MainWidget == nullptr || !IsValid(Character) || !Character->HasActorBegunPlay())
	{
		return;
	}

	if (bIsWidgetsBinded)
	{
		return;
	}

	const auto PrimaryAttrEnum = StaticEnum<EOPrimaryAttr>();
	check(PrimaryAttrEnum);
	for (int32 i = 0; i < PrimaryAttrEnum->NumEnums() - 2; i++)
	{
		const EOPrimaryAttr PrimaryAttr = static_cast<EOPrimaryAttr>(i);
		Character->BindOnChangePrimaryAttribute(PrimaryAttr, MainWidget->GetAttrWidget(PrimaryAttr), MainWidget->GetAttrEventName(PrimaryAttr), true);
	}
	Character->BindOnChangePrimaryAttribute(EOPrimaryAttr::Die, MainWidget.Get(), AnimDiedName);
	Character->BindOnChangePrimaryAttribute(EOPrimaryAttr::Health, MainWidget.Get(), OnHealthChangedName);
	Character->OnChangeAiming.AddDynamic(MainWidget.Get(), &UOMainWidget::OnChangeAiming);
	Character->GetOnNotifyChangeWeapon().AddUObject(MainWidget.Get(), &UOMainWidget::OnNotifyChangeWeapon);
	Character->GetOnNotifyUpdatedAmmoWeapon().AddUObject(MainWidget.Get(), &UOMainWidget::OnNotifyUpdatedAmmoWeapon);
	
	bIsWidgetsBinded = true;
}

void UOWidgetManagerComponent::InitWidgets()
{
	if ((MainWidgetTemplate != nullptr && MainWidget == nullptr))
	{
		MainWidget = CreateWidget<UOMainWidget>(GetWorld(), MainWidgetTemplate.Get());
		if (!MainWidget.IsValid())
		{
			return;
		}
		MainWidget->AddToViewport();
	}
}
