#include "OBaseUtils.h"

#include "OriginPlayerState.h"

bool OBaseUtils::AreTheyEnemies(AController* FirstController, AController* SecondController)
{
	if (!FirstController || !SecondController || FirstController == SecondController)
	{
		return false;
	}

	const AOriginPlayerState* FPlayerState = FirstController->GetPlayerState<AOriginPlayerState>();
	const AOriginPlayerState* SPlayerState = SecondController->GetPlayerState<AOriginPlayerState>();
	return FPlayerState && SPlayerState && FPlayerState->GetTeamId() != SPlayerState->GetTeamId();
}
