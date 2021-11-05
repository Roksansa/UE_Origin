// Fill out your copyright notice in the Description page of Project Settings.


#include "OLadderInteractiveActor.h"

#include "Components/BoxComponent.h"
#include "Origin/OTypes.h"

AOLadderInteractiveActor::AOLadderInteractiveActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);
	
	LeftRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftRail"));
	LeftRailMeshComponent->SetupAttachment(RootComponent);

	RightRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightRail"));
	RightRailMeshComponent->SetupAttachment(RootComponent);

	StepsMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Steps"));
	StepsMeshComponent->SetupAttachment(RootComponent);
}

void AOLadderInteractiveActor::OnConstruction(const FTransform& Transform)
{
	if (StepsInterval < 0.01 || Height< 0.01f || Width < 0.01f)
	{
		return;
	}
	LeftRailMeshComponent->SetRelativeLocation(FVector(0.f,-Width*0.5f,Height*0.5f));
	RightRailMeshComponent->SetRelativeLocation(FVector(0.f,Width*0.5f,Height*0.5f));

	UStaticMesh* LeftRailMesh = LeftRailMeshComponent->GetStaticMesh();
	if (IsValid(LeftRailMesh))
	{
		const float MeshHeight = LeftRailMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			LeftRailMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, Height / MeshHeight));
		}
	}

	UStaticMesh* RightRailMesh = RightRailMeshComponent->GetStaticMesh();
	if (IsValid(RightRailMesh))
	{
		const float MeshHeight = RightRailMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			RightRailMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, Height / MeshHeight));
		}
	}

	UStaticMesh* StepMesh = StepsMeshComponent->GetStaticMesh();
	if (IsValid(StepMesh))
	{
		const float MeshWidth = StepMesh->GetBoundingBox().GetSize().Y;
		if (!FMath::IsNearlyZero(MeshWidth))
		{
			StepsMeshComponent->SetRelativeScale3D(FVector(1.f, Width / MeshWidth, 1.f));
		}
	}

	StepsMeshComponent->ClearInstances();
	
	if (Height > BottomStepOffset)
	{
		const uint32 StepsCount = FMath::FloorToInt((Height - BottomStepOffset) / StepsInterval);

		for (uint32 i = 0; i <= StepsCount; i++)
		{
			FTransform InstanceTransform(FVector(1.f, 1.f, BottomStepOffset + i*StepsInterval));
			StepsMeshComponent->AddInstance(InstanceTransform);
		}
	}

	UBoxComponent* InteractionBox = StaticCast<UBoxComponent*>(InteractionVolume);
	const float BoxDepthExtend = InteractionBox->GetUnscaledBoxExtent().X;
	InteractionBox->SetBoxExtent(FVector(BoxDepthExtend, Width*0.5f, Height * 0.5f));
	InteractionBox->SetRelativeLocation(FVector(BoxDepthExtend, 0, Height * 0.5f));
}

float AOLadderInteractiveActor::GetHeight() const
{
	return Height;
}

float AOLadderInteractiveActor::GetStepsInterval() const
{
	return StepsInterval;
}
