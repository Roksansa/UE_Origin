// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "PlatformInvocator.generated.h"

UCLASS()
class ORIGIN_API APlatformInvocator : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//set value in blueprint construction script
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Config")
	class UBoxComponent* BoxCollider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
	class ABasePlatform* CurrentPlatform;	
};
