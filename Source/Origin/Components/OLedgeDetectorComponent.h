// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OLedgeDetectorComponent.generated.h"


USTRUCT(BlueprintType)
struct FLedgeDescription
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge Desc")
	FVector Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge Desc")
	FRotator Rotation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge Desc")
	FVector LedgeNormal;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORIGIN_API UOLedgeDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool TryDetectLedge(OUT FLedgeDescription& LedgeDescription);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection Setting", meta = (UIMin = 0.f, ClampMin = 0.f))
	float MinLedgeHeight = 40.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection Setting", meta = (UIMin = 0.f, ClampMin = 0.f))
	float MaxLedgeHeight = 200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection Setting", meta = (UIMin = 0.f, ClampMin = 0.f))
	float ForwardCheckDistance = 100.f;

private:
	TWeakObjectPtr<class ACharacter> CachedCharacterOwner;
};
