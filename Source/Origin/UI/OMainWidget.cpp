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

void UOMainWidget::Init()
{
	if (!IsShowPlayerSpec)
	{
		PlayAnimationForward(ShowPlayerSpec);
		IsShowPlayerSpec = true;
	}
	if (IsShowSpectatorPanel)
	{
		PlayAnimationReverse(ShowSpectatorPanel);
		IsShowSpectatorPanel = false;
	}
	if (!IsShowStats)
	{
		PlayAnimationForward(ShowStats);
		IsShowStats = true;
	}
	if (IsPause)
	{
		OnPauseGame(false);
	}
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

void UOMainWidget::OnNotifyUpdatedAmmoWeapon(EOAmmoType Type, int CurrentCount, int MaxCount, bool Infinity)
{
	if (AmmoDescWidget)
	{
		AmmoDescWidget->UpdateAmmoWeapon(Type, CurrentCount, MaxCount, Infinity);
	}
}

void UOMainWidget::OnHideAll(bool bHide)
{
	if (bHide != IsHideAll)
	{
		IsHideAll = bHide;
		if (bHide)
		{
			PlayAnimationForward(HideAll);
			UE_LOG(LogEngine, Warning, TEXT(" HideAll True"));
		}
		else
		{
			if (IsShowPlayerSpec)
			{
				PlayAnimationForward(ShowPlayerSpec);
			}
			if (IsShowSpectatorPanel)
			{
				PlayAnimationForward(ShowSpectatorPanel);
			}
			if (IsShowStats)
			{
				PlayAnimationForward(ShowStats);
			}
			UE_LOG(LogEngine, Warning, TEXT(" HideAll FALSE"));
		}
	}
}

void UOMainWidget::OnPauseGame(bool bPause)
{
	if (bPause != IsPause)
	{
		IsPause = bPause;
		OnHideAll(bPause);
		if (bPause)
		{
			PlayAnimationForward(ShowPauseGame);
			UE_LOG(LogEngine, Warning, TEXT("True"));
		}
		else
		{
			PlayAnimationReverse(ShowPauseGame);
			UE_LOG(LogEngine, Warning, TEXT("false"));
		}
	}
}

void UOMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!AmmoDescWidget)
	{
		AmmoDescWidget = WidgetTree->FindWidget<UOAmmoDescWidget>(AmmoDescWidgetName);
	}
}

void UOMainWidget::AnimDied()
{
	IsShowPlayerSpec = false;
	PlayAnimationReverse(ShowPlayerSpec);
	IsShowSpectatorPanel = true;
	PlayAnimationForward(ShowSpectatorPanel);
}

void UOMainWidget::OnHealthChanged(float CurrentValue, float Diff, float MaxValue)
{
	if (Diff < 0)
	{
		PlayAnimationForward(ShowBlood);
	}
}

UOPlayerStatsWidget* UOMainWidget::GetPlayerStatsWidget()
{
	return WBP_Stats;
}

UOSpectatorWidget* UOMainWidget::GetSpectatorWidget()
{
	return WBP_Spectator;
}
