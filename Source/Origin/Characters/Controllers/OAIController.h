// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "OAIController.generated.h"

class UOAIPerceptionComponent;

UCLASS()
class ORIGIN_API AOAIController : public AAIController
{
	GENERATED_BODY()
public:
	AOAIController();

	const UOAIPerceptionComponent* GetAIPerceptionComponent() const;
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	void UpdateRotation();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AI")
	UOAIPerceptionComponent* AIPerceptionComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AI")
	float RateUpdateActor = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FName EnemyKeyInBlackboard = "FoundEnemyActor";
private:
	FTimerHandle FindActorTimerHandle;
};
