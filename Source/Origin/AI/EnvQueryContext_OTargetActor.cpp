// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_OTargetActor.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"


void UEnvQueryContext_OTargetActor::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());
	UBlackboardComponent* Blackboard = UAIBlueprintHelperLibrary::GetBlackboard(QueryOwner);
	if (!Blackboard)
	{
		return;
	}
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKeyInBlackboard));
	if (TargetActor)
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, QueryOwner);
	}
}
