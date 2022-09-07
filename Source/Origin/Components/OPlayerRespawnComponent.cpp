// Fill out your copyright notice in the Description page of Project Settings.


#include "OPlayerRespawnComponent.h"

#include "OriginGameModeBase.h"

// Sets default values for this component's properties
UOPlayerRespawnComponent::UOPlayerRespawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOPlayerRespawnComponent::Respawn(int32 LeftTime_In)
{
	LeftTime = LeftTime_In;
	OnUpdateRespawnTime.Broadcast(LeftTime);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UOPlayerRespawnComponent::UpdateTimer, TimerInRate, true);
}

void UOPlayerRespawnComponent::StopRespawn()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UOPlayerRespawnComponent::UpdateTimer()
{
	LeftTime -= TimerInRate;
	OnUpdateRespawnTime.Broadcast(LeftTime);
	if (LeftTime <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		if (AOriginGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AOriginGameModeBase>())
		{
			GameMode->Respawn(Cast<AController>(GetOwner()));
		}
	}
}
