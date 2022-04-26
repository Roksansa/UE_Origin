// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OAmmoDescWidget.h"
#include "OPlayerStatsWidget.h"
#include "OSpectatorWidget.h"
#include "OTypes.h"
#include "Actors/Weapon/OBaseWeapon.h"
#include "Blueprint/UserWidget.h"
#include "OMainWidget.generated.h"

class UOCrossHairWidget;

UCLASS()
class ORIGIN_API UOMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	class UOPrimaryAttrWidget* GetAttrWidget(EOPrimaryAttr Type);
	FName GetAttrEventName(EOPrimaryAttr Type);
	void Init();
	
	UFUNCTION()
	void OnChangeAiming(bool bIsAiming);
	void OnNotifyChangeWeapon(EOEquippableItemType EquippableItem);
	void OnNotifyUpdatedAmmoWeapon(EOAmmoType Type, int CurrentCount, int MaxCount, bool Infinity);

	void OnHideAll(bool bHide);
	void OnPauseGame(bool bPause);

	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void AnimDied();
	UFUNCTION()
	void OnHealthChanged(float CurrentValue, float Diff, float MaxValue);

	UOPlayerStatsWidget* GetPlayerStatsWidget();
	UOSpectatorWidget* GetSpectatorWidget();

	FOnInputAction InputPause;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetName")
	FName HealthWidgetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetName")
	FName StaminaWidgetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetName")
	FName AmmoDescWidgetName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="EventName")
	FName OnHealthChangedName = FName("OnPrimaryAttrChanged");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="EventName")
	FName OnStaminaChangedName = FName("OnPrimaryAttrChanged");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WidgetName")
	TMap<EOEquippableItemType, UOCrossHairWidget*> CrossHairWidgets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta=(BindWidgetAnim))
	UWidgetAnimation* ShowPlayerSpec;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta=(BindWidgetAnim))
	UWidgetAnimation* ShowBlood;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta=(BindWidgetAnim))
	UWidgetAnimation* HideAll;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta=(BindWidgetAnim))
	UWidgetAnimation* ShowPauseGame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta=(BindWidgetAnim))
	UWidgetAnimation* ShowSpectatorPanel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta=(BindWidgetAnim))
	UWidgetAnimation* ShowStats;

	UPROPERTY(BlueprintReadOnly, Category = "Config", meta=(BindWidget))
	UOPlayerStatsWidget* WBP_Stats;
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta=(BindWidget))
	UOSpectatorWidget* WBP_Spectator;
private:
	UPROPERTY()
	UOCrossHairWidget* CurrentCrossHairWidget;

	UPROPERTY()
	UOAmmoDescWidget* AmmoDescWidget;

	bool IsShowSpectatorPanel = false;
	bool IsShowPlayerSpec = true;
	bool IsShowStats = true;
	bool IsPause = false;
	bool IsHideAll = false;
};
