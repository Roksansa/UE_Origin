// Fill out your copyright notice in the Description page of Project Settings.


#include "OLedgeDetectorComponent.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Origin/OGameInstance.h"
#include "Origin/OTypes.h"
#include "Origin/Subsystems/ODebugSubsystem.h"
#include "Origin/Utils/OTraceUtils.h"


void UOLedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetOwner()->IsA<ACharacter>(), TEXT("UOLedgeDetectorComponent work only with character"));
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());
}


bool UOLedgeDetectorComponent::TryDetectLedge(FLedgeDescription& LedgeDescription)
{
	if (!CachedCharacterOwner.IsValid())
	{
		return false;		
	}
	UCapsuleComponent* CapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();
	const float BottomOffsetZ = 2.f;
	float CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	float CapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	FVector CharacterBottom = CachedCharacterOwner->GetActorLocation() - (CapsuleHalfHeight - BottomOffsetZ) * FVector::UpVector;

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());
	
	bool bDebugDraw = false;
	float DrawTime = 10.f;
#if ENABLE_DRAW_DEBUG
	UODebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UODebugSubsystem>();
	bDebugDraw = DebugSubsystem->IsCategoryEnabled(DebugCategory_LedgeDetection);
#endif
	
	float ForwardCheckCapsuleRadius = CapsuleRadius;
	float ForwardCheckCapsuleHalfHeight = (MaxLedgeHeight - MinLedgeHeight) * 0.5f;
	FVector ForwardStartLocation = CharacterBottom + (MinLedgeHeight + ForwardCheckCapsuleHalfHeight) * FVector::UpVector;
	FVector ForwardEndLocation = ForwardStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;
	FHitResult ForwardCheckHitResult;
	
	if (!OTraceUtils::SweepCapsuleSingleByChannel(GetWorld(),ForwardCheckHitResult, ForwardStartLocation, ForwardEndLocation, ForwardCheckCapsuleRadius, ForwardCheckCapsuleHalfHeight, FQuat::Identity, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bDebugDraw, DrawTime))
	{
		return false;
	}

	float DownwardSphereRadius = CapsuleRadius;
	float DownwardCheckDepthOffset = 10.f;
	FVector DownwardStartLocation = ForwardCheckHitResult.ImpactPoint - ForwardCheckHitResult.ImpactNormal * DownwardCheckDepthOffset;
	DownwardStartLocation.Z = CharacterBottom.Z + MaxLedgeHeight + DownwardSphereRadius;

	FVector DownwardEndLocation = FVector(DownwardStartLocation.X, DownwardStartLocation.Y, CharacterBottom.Z);
	FHitResult DownwardCheckHitResult;

	if (!OTraceUtils::SweepSphereSingleByChannel(GetWorld(), DownwardCheckHitResult, DownwardStartLocation, DownwardEndLocation, DownwardSphereRadius, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bDebugDraw, DrawTime))
	{
		return false;
	}
	
	float OverlapCapsuleFloorOffset = 2.f;
	FVector OverlapLocation = DownwardCheckHitResult.ImpactPoint + (CapsuleHalfHeight + OverlapCapsuleFloorOffset) * FVector::UpVector;

	if (OTraceUtils::OverlapCapsuleBlockingByProfile(GetWorld(), OverlapLocation, CapsuleRadius, CapsuleHalfHeight, FQuat::Identity, CollisionProfilePawn, QueryParams, bDebugDraw, DrawTime))
	{		
		return false;
	}

	float CoefCapsuleSize = 0.5f;
	int OffsetFromWall = 5;
	float OffsetX = ForwardCheckHitResult.ImpactPoint.X - CharacterBottom.X;
	float OffsetY = ForwardCheckHitResult.ImpactPoint.Y - CharacterBottom.Y;
	float CheckCapsuleHalfHeight = (DownwardCheckHitResult.ImpactPoint.Z - ForwardCheckHitResult.ImpactPoint.Z) * 0.7f;
	FVector CheckLocation = ForwardCheckHitResult.ImpactPoint - (OffsetX + OffsetFromWall * FMath::Sign(OffsetX))*FVector::ForwardVector * CoefCapsuleSize - (OffsetY + OffsetFromWall * FMath::Sign(OffsetY))*FVector::RightVector * CoefCapsuleSize  + (CheckCapsuleHalfHeight) * FVector::UpVector;

	if (OTraceUtils::OverlapCapsuleBlockingByProfile(GetWorld(), CheckLocation, CapsuleRadius * CoefCapsuleSize, CheckCapsuleHalfHeight, FQuat::Identity, CollisionProfilePawn, QueryParams, bDebugDraw, DrawTime))
	{		
		return false;
	}
	
	LedgeDescription.Location = OverlapLocation;
	LedgeDescription.Rotation = (ForwardCheckHitResult.ImpactNormal * FVector(-1.f, -1.f, 0.f)).ToOrientationRotator();
	LedgeDescription.LedgeNormal = ForwardCheckHitResult.ImpactNormal.GetSafeNormal();
	return true;
}
