// Fill out your copyright notice in the Description page of Project Settings.


#include "OInteractiveActor.h"

#include "Components/CapsuleComponent.h"
#include "Origin/Characters/OBaseCharacter.h"

void AOInteractiveActor::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(InteractionVolume))
	{
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AOInteractiveActor::OnInteractionVolumeBeginOverlap);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AOInteractiveActor::OnInteractionVolumeEndOverlap);
	}
}

void AOInteractiveActor::OnInteractionVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AOBaseCharacter* BaseCharacter = Cast<AOBaseCharacter>(OtherActor);
	if (!IsValid(BaseCharacter))
	{
		return;
	}
	if (OtherComp != BaseCharacter->GetCapsuleComponent())
	{
		return;
	}
	BaseCharacter->RegisterInteractiveActor(this);
}

void AOInteractiveActor::OnInteractionVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AOBaseCharacter* BaseCharacter = Cast<AOBaseCharacter>(OtherActor);
	if (!IsValid(BaseCharacter))
	{
		return;
	}
	if (OtherComp != BaseCharacter->GetCapsuleComponent())
	{
		return;
	}
	BaseCharacter->UnregisterInteractiveActor(this);
}