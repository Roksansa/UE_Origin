// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_OTargetActor.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UEnvQueryContext_OTargetActor : public UEnvQueryContext
{
	GENERATED_BODY()
	
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FName TargetActorKeyInBlackboard = "FoundEnemyActor";
};
