// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlatform.h"
#include "Components/SceneComponent.h"
#include "Components/TimelineComponent.h"
// Sets default values
ABasePlatform::ABasePlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PlatformRoot"));
	RootComponent = DefaultPlatformRoot;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	PlatformMesh->SetupAttachment(DefaultPlatformRoot);
}

// Called when the game starts or when spawned
void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = PlatformMesh->GetRelativeLocation();

	InitPlatformBehavior();
}

void ABasePlatform::InitPlatformBehavior()
{
	if (IsValid(MovingCurve))
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this,  FName("MovingPlatform"));
		MovingTimeline.AddInterpFloat(MovingCurve, TimelineCallback);

		FOnTimelineEvent TimelineFinishedEvent;
		TimelineFinishedEvent.BindUFunction(this, FName("SetInformationByFinishedTimeline"));
		MovingTimeline.SetTimelineFinishedFunc(TimelineFinishedEvent);

		MovingTimeline.SetPlayRate(FMath::IsNearlyEqual(MovingTimeline.GetPlaybackPosition(), 0.f) ? ForwardPlayRate : BackwardPlayRate);

		if (PlatformBehavior == EPlatformBehavior::Loop)
		{
			StartMovingPlatform();
		}
	}
}


// Called every frame
void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MovingTimeline.TickTimeline(DeltaTime);
}

void ABasePlatform::MovingPlatform(float Value)
{
	FVector LocalLocation = GetActorLocation();
	//direction
	const FVector NewLocation = StartLocation + Value * (EndLocation - StartLocation);
	PlatformMesh->SetRelativeLocation(NewLocation);
}

void ABasePlatform::SetInformationByFinishedTimeline()
{
	//switch playrate timeline by direction
	const bool IsForward = FMath::IsNearlyEqual(MovingTimeline.GetPlaybackPosition(),0.f);
	MovingTimeline.SetPlayRate(IsForward ? ForwardPlayRate : BackwardPlayRate);
	
	//and start now if loop platform
	if (PlatformBehavior == EPlatformBehavior::Loop)
	{
		if (bOnTimer)
		{
			GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &ABasePlatform::StartMovingPlatformByTimer, DelayMovingByTimer, false);
		}
		else
		{
			StartMovingPlatform();
		}
	}
	if (PlatformBehavior == EPlatformBehavior::OnDemand)
	{
		if (CooldownTimer.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(CooldownTimer);
		}
		if (bOnTimer)
		{
			GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &ABasePlatform::StartMovingPlatformByTimer, DelayMovingByTimer, false);
			bOnTimer = false;
		}
	}
}

bool ABasePlatform::StartMovingPlatform()
{
	if (MovingTimeline.IsPlaying())
	{
		return false;
	}

	//clear timer because timer old or it is not needed
	if (CooldownTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CooldownTimer);
	}
	
	const bool IsForward = FMath::IsNearlyEqual(MovingTimeline.GetPlaybackPosition(),0.f);
	if (IsForward)
	{
		MovingTimeline.Play();
	}
	else
	{
		MovingTimeline.Reverse();
	}
	return true;
}

void ABasePlatform::StartMovingPlatformByTimer()
{
	//ignored return value
	StartMovingPlatform();
}

void ABasePlatform::MovePlatformCalledByInvocator()
{
	const bool IsStarting = StartMovingPlatform();

	if (!IsStarting)
	{
		return;
	}

	if (PlatformBehavior == EPlatformBehavior::OnDemand)
	{
		if (CooldownTimer.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(CooldownTimer);
		}
		bOnTimer = true;
	}
	
	if (OnPlatformMoved.IsBound())
	{
		OnPlatformMoved.Broadcast();
	}
}
