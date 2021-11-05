// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "BasePlatform.generated.h"

UENUM()
enum class EPlatformBehavior : uint8
{
	OnDemand = 0, //платформа перемещается только при запросе в заданном направлении до конца
    Loop // платформа перемещается туда-обратно без триггера.
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlatormMoved);

UCLASS()
class ORIGIN_API ABasePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePlatform();
	
	void InitPlatformBehavior();

	void MovePlatformCalledByInvocator();

	FORCEINLINE EPlatformBehavior GetTypePlatformBehavior() const { return PlatformBehavior;}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
	FVector EndLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	FVector StartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) 
	EPlatformBehavior PlatformBehavior = EPlatformBehavior::OnDemand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* MovingCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOnTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DelayMovingByTimer = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config", meta = (UIMin = 0.01f, ClampMin = 0.01f))
	float ForwardPlayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config", meta = (UIMin = 0.01f, ClampMin = 0.01f))
	float BackwardPlayRate = 1.0f;

	UFUNCTION(BlueprintCallable)
	bool StartMovingPlatform();

	UFUNCTION(BlueprintCallable)
    void StartMovingPlatformByTimer();
	
	UPROPERTY(BlueprintAssignable)
	FOnPlatormMoved OnPlatformMoved;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void MovingPlatform(float Value);

	FTimerHandle CooldownTimer;
	
	FTimeline MovingTimeline;
	
	UFUNCTION()
	void SetInformationByFinishedTimeline();
};
