// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "OAIPerceptionComponent.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UOAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()

public:
	AActor* GetNearestActor() const;
};
