// Fill out your copyright notice in the Description page of Project Settings.


#include "OBaseCharacterAnimInstance.h"

#include "DrawDebugHelpers.h"
#include "Components/OCharacterIKComponent.h"
#include "Components/OWeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Origin/Actors/OLadderInteractiveActor.h"
#include "Origin/Characters/OBaseCharacter.h"
#include "Origin/MovementComponents/OBaseCharacterMovementComponent.h"

void UOBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AOBaseCharacter>(),
		TEXT("UOBaseCharacterAnimInstance::NativeBeginPlay() work with only AOBaseCharacter and him child"));
	CurrentCharacter = StaticCast<AOBaseCharacter*>(TryGetPawnOwner());
	IKComp = StaticCast<UOCharacterIKComponent*>(CurrentCharacter->GetIKComponent());
	LastZLocationForCharacter = 0.f;
}

void UOBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CurrentCharacter.IsValid())
	{
		return;
	}

	UOBaseCharacterMovementComponent* CharacterMovementComponent = CurrentCharacter->GetBaseCharacterMovementComponent();
	Speed = CharacterMovementComponent->Velocity.Size();
	bIsFalling = CharacterMovementComponent->IsFalling();
	bIsCrouching = CharacterMovementComponent->IsCrouching();
	bIsSprinting = CurrentCharacter->bIsSprinting && !CharacterMovementComponent->IsSwimming();
	bIsOutOfStaminaState = CurrentCharacter->GetIsOutOfStamina();
	bIsCrawling = CurrentCharacter->GetIsCrawling();
	bIsSwimming = CharacterMovementComponent->IsSwimming() && !CurrentCharacter->GetIsOverlapVolumeSurface();
	bIsSwimmingOnSurface = CharacterMovementComponent->IsSwimming() && CurrentCharacter->GetIsOverlapVolumeSurface();
	bIsOnLadder = CharacterMovementComponent->IsClimbingLadder();
	ClimbingLadderSpeedRatio = bIsOnLadder ? CharacterMovementComponent->GetClimbingLadderSpeedRatio() : Speed;

	WeaponItemType = CurrentCharacter->GetWeaponComponent()->GetWeaponType();
	bIsAiming = CurrentCharacter->GetWeaponComponent()->GetAiming();
	
	CalcDirection();
	CalcAimRotation(CurrentCharacter->GetBaseAimRotation());

	OffsetRotatorBoneCrouch = FRotator::ZeroRotator;
	if (!bIsOnLadder)
	{
		bCheckedMeshPositionOnLadder = false;
	}
	if (!bIsCrawling && !bIsFalling && !bIsSwimming && !bIsSwimmingOnSurface)
	{
		CalculateIKFootsOffset(DeltaSeconds);
	}
	else if (bIsCrawling)
	{
		CalculateSlopeFloorForCurrentPawn(DeltaSeconds);
	}
	else
	{
		CalculateOffsetRootBoneInZ(DeltaSeconds, 0.f);
	}
}

void UOBaseCharacterAnimInstance::CalcDirection()
{
	if (CurrentCharacter->GetVelocity().IsZero())
	{
		Direction = 0.f;
		return;
	}
	const FVector VelocityNormal = CurrentCharacter->GetVelocity().GetSafeNormal();
	const FVector ForwardVector = CurrentCharacter->GetActorForwardVector().GetSafeNormal();
	const float DotProduct = FVector::DotProduct(ForwardVector, VelocityNormal);
	const FVector CrossProduct = FVector::CrossProduct(ForwardVector, VelocityNormal);
	const float Degree = UKismetMathLibrary::DegAcos(DotProduct);
	Direction = FMath::IsNearlyZero(CrossProduct.Z, 0.01f) ? FMath::Abs(Degree) : Degree * FMath::Sign(CrossProduct.Z);
}

void UOBaseCharacterAnimInstance::CalcAimRotation(const FRotator& Rotator)
{
	AimRotation = FRotator::ZeroRotator;
	AimRotation.Yaw = FMath::Sign(Rotator.Pitch) * FMath::Clamp(FMath::Abs(Rotator.Pitch), MinAimYawIn, MaxAimYawIn);
	if (CurrentCharacter->IsPlayerControlled())
	{
		const float ValueClamped = UKismetMathLibrary::MapRangeClamped(FMath::Abs(AimRotation.Yaw), MinAimYawIn,MaxAimYawIn, MinAimPitch, MaxAimPitch);
		CurrentPitch = UKismetMathLibrary::Lerp(CurrentPitch, ValueClamped, LerpSpeed);
		AimRotation.Pitch = CurrentPitch;
	}
}


void UOBaseCharacterAnimInstance::CalculateIKFootsOffsetOnLadder(float DeltaSeconds)
{
	if (Speed > 0.01f)
	{
		bCheckedMeshPositionOnLadder = false;
		CalculateOffsetRootBoneInZ(DeltaSeconds, 0.f);
		IKRightFootEffectorLocation = FVector(0, 0, 0);
		IKLeftFootEffectorLocation = FVector(0, 0, 0);
		IKRightHandEffectorLocation = FVector(0, 0, 0);
		IKLeftHandEffectorLocation = FVector(0, 0, 0);
		return;
	}

	const FVector IKRightFootOffsetVector = IKComp->GetIKOffsetFromLadderStepForSocketName(IKComp->GetRightFootSocketName());
	const FTransform RightFootSocketTransform = CurrentCharacter->GetMesh()->GetSocketTransform(IKComp->GetRightFootSocketName(),RTS_Actor);
	const FRotator RightFootSocketRotator = RightFootSocketTransform.Rotator();
	const FVector RightFootRotatedVector = RightFootSocketRotator.RotateVector(IKRightFootOffsetVector);
	//GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Orange, FString::Format(TEXT(" IK RightFootSocketRotator {0} -- {1} -- {2} "),
	//{RightFootSocketRotator.ToString(), CurrentCharacter->GetMesh()->GetSocketRotation(CurrentCharacter->GetRightFootSocketName()).ToString(), CurrentCharacter->GetActorRotation().ToString()}), true);
	IKRightFootEffectorLocation = FVector(RightFootRotatedVector.Y, RightFootRotatedVector.X, -RightFootRotatedVector.Z);

	const FVector IKLeftFootOffsetVector = IKComp->GetIKOffsetFromLadderStepForSocketName(IKComp->GetLeftFootSocketName());
	const FTransform LeftFootSocketTransform = CurrentCharacter->GetMesh()->GetSocketTransform(IKComp->GetLeftFootSocketName(),RTS_Actor);
	const FRotator LeftFootSocketRotator = LeftFootSocketTransform.Rotator();
	const FVector LeftFootRotatedVector = LeftFootSocketRotator.RotateVector(IKLeftFootOffsetVector);
	IKLeftFootEffectorLocation = FVector(-LeftFootRotatedVector.Y, LeftFootRotatedVector.X, -LeftFootRotatedVector.Z);

	CalculateOffsetRootBoneInZ(DeltaSeconds, 0);

	IKRightHandOffset = IKComp->GetIKHandOffsetFromLadderStepForSocketName(IKComp->GetRightHandSocketName());
	const FTransform RightHandSocketTransform = CurrentCharacter->GetMesh()->GetSocketTransform(IKComp->GetRightHandSocketName(),RTS_Actor);
	const FRotator RightHandSocketRotator = RightHandSocketTransform.Rotator();
	FVector RightHandRotatedVector = RightHandSocketRotator.RotateVector(IKRightHandOffset);
	IKRightHandEffectorLocation = FVector(-RightHandRotatedVector.Y, -RightHandRotatedVector.Z, -RightHandRotatedVector.X);

	IKLeftHandOffset = IKComp->GetIKHandOffsetFromLadderStepForSocketName(IKComp->GetLeftHandSocketName());
	const FTransform LeftHandSocketTransform = CurrentCharacter->GetMesh()->GetSocketTransform(IKComp->GetLeftHandSocketName(),RTS_Actor);
	const FRotator LeftHandSocketRotator = LeftHandSocketTransform.Rotator();
	FVector LeftHandRotatedVector = LeftHandSocketRotator.RotateVector(IKLeftHandOffset);
	IKLeftHandEffectorLocation = FVector(-LeftHandRotatedVector.Z, LeftHandRotatedVector.Y, -LeftHandRotatedVector.X);
}

void UOBaseCharacterAnimInstance::CalculateIKFootsOffset(float DeltaSeconds)
{
	if (bIsOnLadder)
	{
		CalculateIKFootsOffsetOnLadder(DeltaSeconds);
		return;
	}
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, IKComp->GetIKOffsetForSocketName(IKComp->GetRightFootSocketName()), DeltaSeconds, IKComp->GetIKInterpSpeed());
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, IKComp->GetIKOffsetForSocketName(IKComp->GetLeftFootSocketName()), DeltaSeconds, IKComp->GetIKInterpSpeed());
	if (!bIsCrouching)
	{
		//it is necessary to move location with right leg offset
		const bool bIsRightOffset = IKRightFootOffset > IKLeftFootOffset;
		const bool bNeedChangeLocation = true;

		CalculateOffsetRootBoneInZ(DeltaSeconds, bNeedChangeLocation ? (bIsRightOffset ? IKRightFootOffset : IKLeftFootOffset) : 0.f);

		const float AdditionalOffsetForRightFoot = bNeedChangeLocation ? (bIsRightOffset ? -IKRightFootOffset : -IKLeftFootOffset) : 0.f;
		const float AdditionalOffsetForLeftFoot = bNeedChangeLocation ? (bIsRightOffset ? IKRightFootOffset : IKLeftFootOffset) : 0.f;

		IKRightFootEffectorLocation = FVector(IKRightFootOffset + AdditionalOffsetForRightFoot, 0, 0);
		IKLeftFootEffectorLocation = FVector(-IKLeftFootOffset + AdditionalOffsetForLeftFoot, 0, 0);
	}
	else
	{
		float RealOffsetForRightFoot = IKRightFootOffset;
		float RealOffsetForLeftFoot = IKLeftFootOffset;

		float RealOffsetForMesh = 0.f;
		const bool bNeedZLocationDown = MaxDistanceLegUpAfterChangeZLocationByCrouched - IKRightFootOffset > 0.f ||
		                                MaxDistanceLegUpAfterChangeZLocationByCrouched - IKLeftFootOffset > 0.f;
		if (bNeedZLocationDown)
		{
			if (MaxDistanceLegUpAfterChangeZLocationByCrouched - IKRightFootOffset > 0.f)
			{
				RealOffsetForMesh = MaxDistanceLegUpAfterChangeZLocationByCrouched - IKRightFootOffset;
			}
			else
			{
				RealOffsetForMesh = MaxDistanceLegUpAfterChangeZLocationByCrouched - IKLeftFootOffset;
			}
			RealOffsetForRightFoot += RealOffsetForMesh;
			RealOffsetForLeftFoot += RealOffsetForMesh;
		}
		const bool bNeedChangeLocation = true;
		CalculateOffsetRootBoneInZ(DeltaSeconds, bNeedChangeLocation ? -RealOffsetForMesh : 0.f);

		IKRightFootEffectorLocation = FVector(RealOffsetForRightFoot, 0, 0);
		IKLeftFootEffectorLocation = FVector(-RealOffsetForLeftFoot, 0, 0);
	}
}

void UOBaseCharacterAnimInstance::CalculateOffsetRootBoneInZ(float DeltaSeconds, const float OffsetForFoot)
{
	const float LocationInZ = - OffsetForFoot;
	LastZLocationForCharacter = FMath::FInterpTo(LastZLocationForCharacter, LocationInZ, DeltaSeconds, IKComp->GetIKInterpSpeed());
	OffsetTransformBoneIdleCrouch = FVector(0, 0, LastZLocationForCharacter);
}

void UOBaseCharacterAnimInstance::CalculateSlopeFloorForCurrentPawn(float DeltaSeconds)
{
	const FFindFloorResult PawnFloor = CurrentCharacter->GetBaseCharacterMovementComponent()->CurrentFloor;
	const FVector PawnForwardDirection = CurrentCharacter->GetActorForwardVector();
	const float DotProduct = FVector::DotProduct(PawnForwardDirection, PawnFloor.HitResult.ImpactNormal);
	const float Degree = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
	const int DiffDegree = 90;
	const float RollWithDiff = -Degree + DiffDegree;
	OffsetRotatorBoneCrouch = FRotator(0, 0, RollWithDiff);
	const float OffsetZ = OffsetZLocationForCrawl * (-RollWithDiff / OffsetZLocationForCrawl);
	CalculateOffsetRootBoneInZ(DeltaSeconds, OffsetZ);
}
