// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformInvocator.h"
#include "Components/BoxComponent.h"
#include "BasePlatform.h"

// Called when the game starts or when spawned
void APlatformInvocator::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(CurrentPlatform) && IsValid(BoxCollider))
	{
		BoxCollider->SetGenerateOverlapEvents(true);
		BoxCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		BoxCollider->bHiddenInGame = false;
		BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &APlatformInvocator::OnOverlapBegin);
	}
}

void APlatformInvocator::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CurrentPlatform->GetTypePlatformBehavior() == EPlatformBehavior::OnDemand)
	{
		CurrentPlatform->MovePlatformCalledByInvocator();
	}
}
