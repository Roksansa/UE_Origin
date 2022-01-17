// Fill out your copyright notice in the Description page of Project Settings.


#include "OCrossHairWidget.h"

void UOCrossHairWidget::SwitchAnimAim(bool bIsAiming)
{
	if (bIsAiming)
	{
		this->PlayAnimationForward(Aiming);
	}
	else
	{
		this->PlayAnimationReverse(Aiming);
	}
}

void UOCrossHairWidget::SwitchAnimHide(bool bIsVisibility)
{
	if (!bIsVisibility)
	{
		this->PlayAnimationForward(Hide);
	}
	else
	{
		this->PlayAnimationReverse(Hide);
	}
}
