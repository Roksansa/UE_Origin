// Fill out your copyright notice in the Description page of Project Settings.


#include "OAIController.h"

#include "AI/Components/OAIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/OAICharacter.h"
#include "Components/OPlayerRespawnComponent.h"

AOAIController::AOAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UOAIPerceptionComponent>("AIPerceptionComponent");
	SetPerceptionComponent(*AIPerceptionComponent);
	RespawnComponent = CreateDefaultSubobject<UOPlayerRespawnComponent>("RespawnComponent");
	bWantsPlayerState = true;
}

const UOAIPerceptionComponent* AOAIController::GetAIPerceptionComponent() const
{
	return AIPerceptionComponent;
}

void AOAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AOAICharacter* AICharacter = Cast<AOAICharacter>(InPawn);
	if (AICharacter)
	{
		RunBehaviorTree(AICharacter->BehaviorTree);
	}
	GetWorldTimerManager().SetTimer(FindActorTimerHandle, this, &AOAIController::UpdateRotation, RateUpdateActor, true);
	UpdateRotation();
}

void AOAIController::OnUnPossess()
{
	Super::OnUnPossess();
	GetWorldTimerManager().ClearTimer(FindActorTimerHandle);
}

void AOAIController::UpdateRotation()
{
	AActor* FindActor = nullptr;
	if (Blackboard)
	{
		FindActor = Cast<AActor>(Blackboard->GetValueAsObject(EnemyKeyInBlackboard));
	}
	if (FindActor)
	{
		SetFocus(FindActor);
	}
}
