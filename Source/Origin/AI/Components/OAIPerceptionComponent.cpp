// Fill out your copyright notice in the Description page of Project Settings.


#include "OAIPerceptionComponent.h"

#include "Characters/OBaseCharacter.h"
#include "Characters/Controllers/OAIController.h"
#include "Perception/AISense_Sight.h"
#include "Utils/OBaseUtils.h"

AActor* UOAIPerceptionComponent::GetNearestActor() const
{
	TArray<AActor*> ResultActors;
	GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), ResultActors);
	if (ResultActors.Num() == 0)
	{
		return nullptr;
	}

	const auto Controller = Cast<AOAIController>(GetOwner());
	if (!Controller)
	{
		return nullptr;
	}
	float MinDistance = MAX_FLT;
	AActor* MinDistancePawn = nullptr;
	FVector CurrentLocation = GetOwner()->GetActorLocation();
	for (const auto ResultActor : ResultActors)
	{
		AOBaseCharacter* CurChar = Cast<AOBaseCharacter>(ResultActor);
		if (!CurChar || CurChar->IsDie())
		{
			continue;
		}
		bool IsEnemy = OBaseUtils::AreTheyEnemies(Controller, CurChar->GetController());
		if (!IsEnemy)
		{
			continue;
		}
		float DistSquared = FVector::DistSquared(ResultActor->GetActorLocation(), CurrentLocation);
		if (DistSquared < MinDistance)
		{
			MinDistance = DistSquared;
			MinDistancePawn = ResultActor;
		}
	}
	return MinDistancePawn;
}
