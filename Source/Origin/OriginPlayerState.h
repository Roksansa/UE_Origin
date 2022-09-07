// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OriginPlayerState.generated.h"

UCLASS()
class ORIGIN_API AOriginPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AOriginPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	int32 GetTeamId() const;

	void SetTeamId(const int32 TeamIdIn);

	FLinearColor GetTeamColor() const;

	void SetTeamColor(const FLinearColor& TeamColorIn);
	void AddKill(bool bIsFriendly = false);
	void AddDeath();
	int32 GetKills() const;
	int32 GetDeaths() const;
private:
	int32 TeamId;
	FLinearColor TeamColor;
	int32 KillsNum;
	int32 DeathsNum;
};
