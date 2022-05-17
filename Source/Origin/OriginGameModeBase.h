// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OTypes.h"
#include "GameFramework/GameModeBase.h"
#include "OriginGameModeBase.generated.h"

DECLARE_MULTICAST_DELEGATE_FourParams(FOnUpdateStateNum, const AController*, const AController*, int32, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUpdateRoundNum, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateTimerLeft, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateMatchState, EOMatchState);
class AAIController;

UCLASS()
class ORIGIN_API AOriginGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartPlay() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual bool SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate) override;
	virtual bool ClearPause() override;
	
	bool IsOnFriendlyFire() const;
	void UpdateKillDeathInfo(AController* DeadController, AController* KillerController);
	int32 GetLeftTime() const;
	void Respawn(AController* Controller);

	void ResetLevelOnGameOver();
	EOMatchState GetMatchState() const;

	/**
	 * first int32 - DeadContoller's deaths, 
	 * second int32 - KillerContoller's kills
	 */
	FOnUpdateStateNum OnUpdateStateNum;
	FOnUpdateTimerLeft OnUpdateTimerLeft;
	FOnUpdateRoundNum OnUpdateRoundNum;
	FOnUpdateMatchState OnUpdateMatchState;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	TSubclassOf<AAIController> AIControllerTemplate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	TSubclassOf<APawn> AIPawnTemplate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	FOGameData GameData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game", meta = (ClampMin = 1, ClampMax = 5))
	int32 TimerRate = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	bool bOnFriendlyFire = false;

private:
	void SpawnAI();
	void UpdateTime();
	void StartRound();
	void ResetPlayers();
	void ResetPlayer(AController* Controller);
	void CreateTeamInfos();
	FLinearColor GetColorByTeamId(int32 TeamId) const;
	void SetPlayerColor(AController* Controller);
	void SetMatchState(EOMatchState NewMatchState);

	int32 CurrentRound = 1;
	int32 LeftTimeInSec = 0;
	FTimerHandle RoundTimerHandle;
	EOMatchState MatchState = EOMatchState::WaitingToStart;
};
