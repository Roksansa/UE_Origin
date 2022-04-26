// Fill out your copyright notice in the Description page of Project Settings.


#include "OWidgetManagerComponent.h"

#include "OPlayerRespawnComponent.h"
#include "OriginGameModeBase.h"
#include "OriginPlayerState.h"
#include "Characters/OBaseCharacter.h"
#include "Characters/Controllers/OPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	Character->GetWeaponComponent()->OnUpdateAmmo();

	MainWidget->Init();
	bIsWidgetsBinded = true;
}

void UOWidgetManagerComponent::InitWidgets(AController* NewController)
{
	if ((MainWidgetTemplate != nullptr && MainWidget == nullptr) && NewController != nullptr)
	{
		MainWidget = CreateWidget<UOMainWidget>(GetWorld(), MainWidgetTemplate.Get());
		if (!MainWidget.IsValid())
		{
			return;
		}
		MainWidget->AddToViewport();
		Controller = Cast<AOPlayerController>(NewController);
		MainWidget->InputPause.BindUFunction(Controller.Get(), "PauseGame");
		UOPlayerStatsWidget* PlayerStatsWidget = MainWidget->GetPlayerStatsWidget();
		if (AOriginGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AOriginGameModeBase>())
		{
			GameMode->OnUpdateStateNum.AddUObject(this, &UOWidgetManagerComponent::UpdateDesc);
			GameMode->OnUpdateRoundNum.AddUObject(PlayerStatsWidget, &UOPlayerStatsWidget::UpdateRounds);
			GameMode->OnUpdateTimerLeft.AddUObject(PlayerStatsWidget, &UOPlayerStatsWidget::UpdateTimer);
			GameMode->OnUpdateMatchState.AddUObject(this, &UOWidgetManagerComponent::UpdateMatchState);
			PlayerStatsWidget->UpdateTimer(GameMode->GetLeftTime());
		}
		PlayerState = Controller->GetPlayerState<AOriginPlayerState>();
		if (PlayerState.IsValid())
		{
			PlayerStatsWidget->UpdatePlayerStats(PlayerState->GetKills(), PlayerState->GetDeaths());
		}

		UOPlayerRespawnComponent* RespawnComponent = Cast<UOPlayerRespawnComponent>(Controller->GetComponentByClass(UOPlayerRespawnComponent::StaticClass()));
		if (RespawnComponent)
		{
			RespawnComponent->OnUpdateRespawnTime.AddUObject(MainWidget->GetSpectatorWidget(), &UOSpectatorWidget::UpdateTimeRespawn);
		}
	}
}

void UOWidgetManagerComponent::UnbindWidgets()
{
	bIsWidgetsBinded = false;
}

void UOWidgetManagerComponent::UpdateDesc(const AController* DeadController, const AController* KillerController, int32 Death, int32 Kills)
{
	if (!Controller.Get() || (DeadController != Controller && KillerController != Controller))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Format(TEXT("UpdateDesc Name1: {0} Deaths : {1} ----- Name2: {2} Kills: {3} "), {DeadController ? DeadController->GetName() : "Null", Death, KillerController ? KillerController->GetName() : "Null2", Kills}));
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Format(TEXT("UpdateDesc Name1: {0} Deaths : {1} ----- Name2: {2} Kills: {3} "), {DeadController ? DeadController->GetName() : "Null", Death, KillerController ? KillerController->GetName() : "Null2", Kills}));
	if (PlayerState.Get())
	{
		UOPlayerStatsWidget* PlayerStatsWidget = MainWidget->GetPlayerStatsWidget();
		PlayerStatsWidget->UpdatePlayerStats(PlayerState->GetKills(), PlayerState->GetDeaths());
	}
}

void UOWidgetManagerComponent::ShowAllStats()
{
	if (Controller.Get())
	{
		Controller->ChangeState(NAME_Default);
	}
	FConstControllerIterator Controllers = GetWorld()->GetControllerIterator();
	for (FConstControllerIterator& It = Controllers; It; ++It)
	{
		AController* CurController = It->Get();
		if (CurController)
		{
			AOriginPlayerState* HisPlayerState = CurController->GetPlayerState<AOriginPlayerState>();
			if (HisPlayerState)
			{
				GEngine->AddOnScreenDebugMessage(-1, 112, FColor::Red, FString::Format(TEXT("Name: {0} ___ ID: {3}, Kills : {1}, Deaths: {2}"),
					{CurController->GetName(), HisPlayerState->GetKills(), HisPlayerState->GetDeaths()}));
			}
		}
	}
	if (MainWidget.IsValid())
	{
		MainWidget->OnHideAll(true);
	}
}

void UOWidgetManagerComponent::UpdateMatchState(EOMatchState MatchState)
{
	if (Controller.IsValid())
	{
		Controller->bShowMouseCursor = MatchState != EOMatchState::InProgress;
		MatchState != EOMatchState::InProgress ? Controller->SetInputMode(FInputModeGameAndUI()) : Controller->SetInputMode(FInputModeGameOnly());
	}
	switch (MatchState)
	{
		case EOMatchState::WaitingToStart: break;
		case EOMatchState::InProgress:
		{
			if (MainWidget.IsValid())
			{
				MainWidget->Init();
			}
			break;
		}
		case EOMatchState::Pause:
		{
			if (MainWidget.IsValid())
			{
				MainWidget->OnPauseGame(true);
			}
			break;
		}
		case EOMatchState::GameOver:
		{
			ShowAllStats();
			break;
		}
		default: ;
	}
}
