// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OPlayerRespawnComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateRespawnTime, int32);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORIGIN_API UOPlayerRespawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOPlayerRespawnComponent();

	void Respawn(int32 LeftTime_In);
	void StopRespawn();
	
	FOnUpdateRespawnTime OnUpdateRespawnTime;
private:
	int32 LeftTime;
	FTimerHandle TimerHandle;
	int32 TimerInRate = 1;

	void UpdateTimer();
};
