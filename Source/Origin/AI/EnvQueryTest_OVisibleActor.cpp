// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryTest_OVisibleActor.h"

#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"
#include "Interfaces/OVisibleInterface.h"

UEnvQueryTest_OVisibleActor::UEnvQueryTest_OVisibleActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Cost = EEnvTestCost::Medium;
	ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_OVisibleActor::RunTest(FEnvQueryInstance& QueryInstance) const
{
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());
		const IOVisibleInterface* VisibleItem = Cast<IOVisibleInterface>(ItemActor);
		if (!VisibleItem)
		{
			continue;
		}
		
		if (VisibleItem->IsVisibleActor())
		{
			It.SetScore(TestPurpose, FilterType, true, true);
		}
		else
		{
			It.ForceItemState(EEnvItemStatus::Failed);
		}
	}
}
