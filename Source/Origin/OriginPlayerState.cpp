// Fill out your copyright notice in the Description page of Project Settings.


#include "OriginPlayerState.h"

AOriginPlayerState::AOriginPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	KillsNum = 0;
	DeathsNum = 0;
}

int32 AOriginPlayerState::GetTeamId() const
{
	return TeamId;
}

void AOriginPlayerState::SetTeamId(const int32 TeamIdIn)
{
	this->TeamId = TeamIdIn;
}

FLinearColor AOriginPlayerState::GetTeamColor() const
{
	return TeamColor;
}

void AOriginPlayerState::SetTeamColor(const FLinearColor& TeamColorIn)
{
	this->TeamColor = TeamColorIn;
}

void AOriginPlayerState::AddKill(bool bIsFriendly)
{
	KillsNum = KillsNum + (bIsFriendly ? -1 : 1);
}

void AOriginPlayerState::AddDeath()
{
	++DeathsNum;
}
int32 AOriginPlayerState::GetKills() const
{
	return KillsNum;
}

int32 AOriginPlayerState::GetDeaths() const
{
	return DeathsNum;
}
