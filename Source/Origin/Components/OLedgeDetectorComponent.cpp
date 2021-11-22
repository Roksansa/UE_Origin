// Fill out your copyright notice in the Description page of Project Settings.


#include "OLedgeDetectorComponent.h"

#include "DrawDebugHelpers.h"
#include "Actors/OLadderInteractiveActor.h"
#include "Characters/OBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "MovementComponents/OBaseCharacterMovementComponent.h"
#include "Origin/OGameInstance.h"
#include "Origin/OTypes.h"
#include "Origin/Subsystems/ODebugSubsystem.h"
#include "Origin/Utils/OTraceUtils.h"


void UOLedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetOwner()->IsA<ACharacter>(), TEXT("UOLedgeDetectorComponent work only with character"));
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UODebugSubsystem>();
#endif
}


bool UOLedgeDetectorComponent::TryDetectLedge(FOLedgeDescription& LedgeDescription)
{
	if (!CachedCharacterOwner.IsValid())
	{
		return false;		
	}
	UCapsuleComponent* CapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();
	const float BottomOffsetZ = 2.f;
	const ACharacter* DefaultChar = GetDefault<ACharacter>(CachedCharacterOwner->GetClass());
	float CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	float CapsuleRadius = DefaultChar->GetCapsuleComponent()->GetScaledCapsuleRadius();
	FVector Location = CachedCharacterOwner->GetActorLocation();
	FVector CharacterBottom = Location - (CapsuleHalfHeight - BottomOffsetZ) * CachedCharacterOwner->GetActorUpVector();

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());
	
	AOBaseCharacter* Character = Cast<AOBaseCharacter>(CachedCharacterOwner);
	if (Character)
	{
		QueryParams.AddIgnoredActor(Character->GetBaseCharacterMovementComponent()->GetCurrentLadder());
	}
	
	bool bDebugDraw = false;
	float DrawTime = 10.f;
#if ENABLE_DRAW_DEBUG
	bDebugDraw = DebugSubsystem->IsCategoryEnabled(DebugCategory_LedgeDetection);
#endif
	
	float ForwardCheckCapsuleRadius = CapsuleRadius;
	float ForwardCheckCapsuleHalfHeight = (MaxLedgeHeight - MinLedgeHeight) * 0.5f;
	FVector ForwardStartLocation = CharacterBottom + (MinLedgeHeight + ForwardCheckCapsuleHalfHeight) * CachedCharacterOwner->GetActorUpVector();
	FVector ForwardEndLocation = ForwardStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;
	FHitResult ForwardCheckHitResult;
	
	if (!OTraceUtils::SweepCapsuleSingleByChannel(GetWorld(),ForwardCheckHitResult, ForwardStartLocation, ForwardEndLocation, ForwardCheckCapsuleRadius, ForwardCheckCapsuleHalfHeight, CachedCharacterOwner->GetActorRotation().Quaternion(), ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bDebugDraw, DrawTime))
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
	CapsuleHalfHeight = DefaultChar->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector OverlapLocation = DownwardCheckHitResult.ImpactPoint + (CapsuleHalfHeight + OverlapCapsuleFloorOffset) * FVector::UpVector;

	if (OTraceUtils::OverlapCapsuleBlockingByProfile(GetWorld(), OverlapLocation, CapsuleRadius, CapsuleHalfHeight, FQuat::Identity, CollisionProfilePawn, QueryParams, bDebugDraw, DrawTime))
	{
		return false;
	}
	
	float CoefCapsuleSize = 0.5f;
	int OffsetFromWall = 5;
	float OffsetX = ForwardCheckHitResult.ImpactPoint.X - Location.X;
	float OffsetY = ForwardCheckHitResult.ImpactPoint.Y - Location.Y;
	float CheckCapsuleHalfHeight = (DownwardCheckHitResult.ImpactPoint.Z - ForwardCheckHitResult.ImpactPoint.Z) * 0.7f;
	FVector CheckLocation = ForwardCheckHitResult.ImpactPoint - (OffsetX + OffsetFromWall * FMath::Sign(OffsetX))*FVector::ForwardVector * CoefCapsuleSize - (OffsetY + OffsetFromWall * FMath::Sign(OffsetY))*FVector::RightVector * CoefCapsuleSize  + (CheckCapsuleHalfHeight) * FVector::UpVector;

	if (OTraceUtils::OverlapCapsuleBlockingByProfile(GetWorld(), CheckLocation, CapsuleRadius * CoefCapsuleSize, CheckCapsuleHalfHeight, FQuat::Identity, CollisionProfilePawn, QueryParams, bDebugDraw, DrawTime))
	{		
		return false;
	}

	FVector BoxExtend = FVector(ForwardCheckCapsuleRadius, ForwardCheckCapsuleRadius, ForwardCheckCapsuleHalfHeight);
	if (!OTraceUtils::SweepBoxSingleByChannel(GetWorld(),ForwardCheckHitResult, ForwardStartLocation, ForwardEndLocation, BoxExtend, FQuat::Identity, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bDebugDraw, DrawTime))
	{
		return false;
	}
	
	
	LedgeDescription.Location = OverlapLocation;
	LedgeDescription.Rotation = (ForwardCheckHitResult.ImpactNormal * FVector(-1.f, -1.f, 0.f)).ToOrientationRotator();
	LedgeDescription.LedgeNormal = ForwardCheckHitResult.ImpactNormal.GetSafeNormal();
	return true;
}
