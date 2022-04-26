// Copyright Epic Games, Inc. All Rights Reserved.


#include "OriginGameModeBase.h"
#include "AIController.h"
#include "OriginPlayerState.h"
#include "Characters/OBaseCharacter.h"
#include "Components/OPlayerRespawnComponent.h"

void AOriginGameModeBase::StartPlay()
{
	Super::StartPlay();
	checkf(GameData.TeamColors.Num() >= GameData.TeamNum, TEXT("GameData.TeamColors.Num() < GameData.TeamNum, Need GameData.TeamNum colors"));
	SpawnAI();
	CreateTeamInfos();
	CurrentRound = 0;
	SetMatchState(EOMatchState::InProgress);
	StartRound();
}

UClass* AOriginGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController && InController->IsA<AAIController>())
	{
		return AIPawnTemplate;
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

bool AOriginGameModeBase::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
	const bool bPause = Super::SetPause(PC, CanUnpauseDelegate);
	if (bPause)
	{
		SetMatchState(EOMatchState::Pause);
	}
	return bPause;
}

bool AOriginGameModeBase::ClearPause()
{
	const bool bClear = Super::ClearPause();
	if (bClear)
	{
		SetMatchState(EOMatchState::InProgress);
	}
	return bClear;
}

bool AOriginGameModeBase::IsOnFriendlyFire() const
{
	return bOnFriendlyFire;
}

void AOriginGameModeBase::UpdateKillDeathInfo(AController* DeadController, AController* KillerController)
{
	int32 TeamIdDead = -1;
	int32 Deaths = -1;
	if (DeadController)
	{
		if (AOriginPlayerState* DeadState = DeadController->GetPlayerState<AOriginPlayerState>())
		{
			DeadState->AddDeath();
			TeamIdDead = DeadState->GetTeamId();
			Deaths = DeadState->GetDeaths();
		}
		UOPlayerRespawnComponent* RespawnComponent = Cast<UOPlayerRespawnComponent>(DeadController->GetComponentByClass(UOPlayerRespawnComponent::StaticClass()));
		if (RespawnComponent)
		{
			if (LeftTimeInSec > GameData.PlayerRespawnTimeInSec)
			{
				RespawnComponent->Respawn(GameData.PlayerRespawnTimeInSec);
			}
		}
	}

	int32 Kills = -1;
	if (DeadController != KillerController)
	{
		if (KillerController)
		{
			if (AOriginPlayerState* KillerState = KillerController->GetPlayerState<AOriginPlayerState>())
			{
				const bool bIsFriendly = TeamIdDead == KillerState->GetTeamId();
				KillerState->AddKill(bIsFriendly);
				Kills = KillerState->GetKills();
			}
		}
	}
	OnUpdateStateNum.Broadcast(DeadController, DeadController != KillerController ? KillerController : nullptr, Deaths, Kills);
}

int32 AOriginGameModeBase::GetLeftTime() const
{
	return LeftTimeInSec;
}

void AOriginGameModeBase::Respawn(AController* Controller)
{
	ResetPlayer(Controller);
}


void AOriginGameModeBase::SpawnAI()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}
	FActorSpawnParameters SpawnInfoParameters;
	SpawnInfoParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (int32 i = 0; i < GameData.PlayersNum - 1; i++)
	{
		AAIController* Controller = GetWorld()->SpawnActor<AAIController>(AIControllerTemplate, SpawnInfoParameters);
	}
}

void AOriginGameModeBase::UpdateTime()
{
	LeftTimeInSec = LeftTimeInSec - TimerRate;
	if (LeftTimeInSec < TimerRate)
	{
		GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(RoundTimerHandle, this, &AOriginGameModeBase::UpdateTime, LeftTimeInSec, false);
	}
	OnUpdateTimerLeft.Broadcast(LeftTimeInSec);
	if (LeftTimeInSec <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);
		if (CurrentRound + 1 <= GameData.RoundsNum)
		{
			StartRound();
		}
		else
		{
			FConstControllerIterator Controllers = GetWorld()->GetControllerIterator();
			for (FConstControllerIterator& It = Controllers; It; ++It)
			{
				AController* Controller = It->Get();
				if (Controller && Controller->GetPawn())
				{
					Controller->GetPawn()->Reset();
				}
			}
			GEngine->AddOnScreenDebugMessage(112, 112, FColor::Blue, FString::Format(TEXT("End Game "), {1}));
			SetMatchState(EOMatchState::GameOver);
		}
	}
}

void AOriginGameModeBase::StartRound()
{
	ResetPlayers();
	CurrentRound++;
	OnUpdateRoundNum.Broadcast(CurrentRound, GameData.RoundsNum);
	LeftTimeInSec = GameData.RoundTimeInSec;
	OnUpdateTimerLeft.Broadcast(LeftTimeInSec);
	GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(RoundTimerHandle, this, &AOriginGameModeBase::UpdateTime, LeftTimeInSec < TimerRate ? LeftTimeInSec : TimerRate, true);
}

void AOriginGameModeBase::ResetPlayers()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}
	FConstControllerIterator Controllers = GetWorld()->GetControllerIterator();
	for (FConstControllerIterator& It = Controllers; It; ++It)
	{
		ResetPlayer(It->Get());
	}
}

void AOriginGameModeBase::ResetPlayer(AController* Controller)
{
	if (Controller && Controller->GetPawn())
	{
		Controller->GetPawn()->Reset();
	}
	RestartPlayer(Controller);
	SetPlayerColor(Controller);
}

void AOriginGameModeBase::CreateTeamInfos()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}
	int32 TeamId = 1;
	FConstControllerIterator Controllers = GetWorld()->GetControllerIterator();
	for (FConstControllerIterator& It = Controllers; It; ++It)
	{
		AController* Controller = It->Get();
		if (!Controller)
		{
			continue;
		}
		AOriginPlayerState* PlayerState = Controller->GetPlayerState<AOriginPlayerState>();
		if (PlayerState)
		{
			PlayerState->SetTeamId(TeamId);
			PlayerState->SetTeamColor(GetColorByTeamId(TeamId));
			SetPlayerColor(Controller);
			TeamId = TeamId % GameData.TeamNum + 1;
		}
	}
}

FLinearColor AOriginGameModeBase::GetColorByTeamId(int32 TeamId) const
{
	if (TeamId > GameData.TeamColors.Num())
	{
		return GameData.TeamColors[0];
	}
	return GameData.TeamColors[TeamId - 1];
}

void AOriginGameModeBase::SetPlayerColor(AController* Controller)
{
	if (!Controller)
	{
		return;
	}
	AOBaseCharacter* Character = Cast<AOBaseCharacter>(Controller->GetPawn());
	AOriginPlayerState* PlayerState = Controller->GetPlayerState<AOriginPlayerState>();
	if (!Character || !PlayerState)
	{
		return;
	}
	Character->SetCharacterColor(PlayerState->GetTeamColor());
}

void AOriginGameModeBase::SetMatchState(EOMatchState NewMatchState)
{
	if (NewMatchState == MatchState)
	{
		return;
	}
	MatchState = NewMatchState;
	OnUpdateMatchState.Broadcast(MatchState);
}
