// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OInteractiveActor.h"
#include "OLadderInteractiveActor.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class ORIGIN_API AOLadderInteractiveActor : public AOInteractiveActor
{
	GENERATED_BODY()
public:
	AOLadderInteractiveActor();
	virtual void OnConstruction(const FTransform& Transform) override;
	
	float GetHeight() const;
	float GetStepsInterval() const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	float Height = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	float Width = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	float StepsInterval = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	float BottomStepOffset = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Configuration")
	UStaticMeshComponent* LeftRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Configuration")
	UStaticMeshComponent* RightRailMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Configuration")
	UInstancedStaticMeshComponent* StepsMeshComponent;
};