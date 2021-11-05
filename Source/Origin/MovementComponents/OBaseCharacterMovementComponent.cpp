// Fill out your copyright notice in the Description page of Project Settings.


#include "OBaseCharacterMovementComponent.h"


#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Character.h"
#include "Origin/Actors/OLadderInteractiveActor.h"
#include "Origin/Characters/OBaseCharacter.h"

bool UOBaseCharacterMovementComponent::CanCrouchInCurrentState() const
{
    //can't crouch in falling or fly etc - only crouch on ground
    if (!IsMovingOnGround())
    {
        return false;
    }
    return Super::CanCrouchInCurrentState();
}

//we suppose that the character is crouching, the collision capsule down to the bottom and it is smaller than the original size
//therefore we use simplified calculations
bool UOBaseCharacterMovementComponent::CanGrowCapsule(bool bClientSimulation) const
{
	if (!HasValidData())
	{
		return false;
	}

	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();

	const float CurrentCrouchedHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	// check grow to uncrouched size.
	check(CharacterOwner->GetCapsuleComponent());

	bool bEncroached = false;
	
	if (!bClientSimulation)
	{
		// Try to stay in place and see if the larger capsule fits. We use a slightly taller capsule to avoid penetration.
		const UWorld* MyWorld = GetWorld();
		const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);

		// Compensate for the difference between current capsule size and standing size
		const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust);
		// Shrink by negative amount, so actually grow it.
		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

		//bCrouchMaintainsBaseLocation = 1 for walking - crouch only ground 
		// Expand while keeping base location the same.
		FVector StandingLocation = PawnLocation + FVector( 0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentCrouchedHalfHeight);
		bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

		if (bEncroached)
		{
			if (IsMovingOnGround())
			{
				// Something might be just barely overhead, try moving down closer to the floor to avoid it.
				const float MinFloorDist = KINDA_SMALL_NUMBER * 10.f;
				if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
				{
					StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
					bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
				}
			}
		}
	}

	return !bEncroached;
}

bool UOBaseCharacterMovementComponent::CanSprint() const
{
	return (IsMovingOnGround() || IsSwimming() || IsClimbingLadder()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

bool UOBaseCharacterMovementComponent::IsSprinting() const
{
	return CachedBaseCharacter && CachedBaseCharacter->bIsSprinting;
}

void UOBaseCharacterMovementComponent::ChangeSprint(bool bStartSprint)
{
	CachedBaseCharacter->bIsSprinting = bStartSprint;
	bRequestedMoveWithMaxSpeed = bStartSprint;
	if (bStartSprint)
	{
		CachedBaseCharacter->OnSprintStart();
	}
	else
	{
		CachedBaseCharacter->OnSprintEnd();
	}
}

bool UOBaseCharacterMovementComponent::CanSprintInCurrentState() const
{
	return !IsOutOfStamina() && !(CachedBaseCharacter && CachedBaseCharacter->GetIsCrawling()) && (IsMovingOnGround() || IsSwimming()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

void UOBaseCharacterMovementComponent::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	Super::SetUpdatedComponent(NewUpdatedComponent);
	if (ensureAlwaysMsgf(PawnOwner && PawnOwner->IsA<AOBaseCharacter>(), TEXT("UOBaseCharacterMovementComponent::SetUpdatedComponent CachedBaseCharacter sprint work only AOBaseCharacter and children")))
	{
		CachedBaseCharacter = StaticCast<AOBaseCharacter*>(PawnOwner);
	}
	else
	{
		CachedBaseCharacter = nullptr;
	}
}

void UOBaseCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	const bool bIsCrouching = IsCrouching();
	const bool bIsCrawling = IsCrawling();
	const bool bWasJumpWithCrawling = bIsCrawling && !bWantsToCrawling && !bWantsToCrouch;
	if (UpdatedComponent && CachedBaseCharacter)
	{
		if (bIsCrouching && !bIsCrawling && bWantsToCrawling && CanCrawlingInCurrentState())
		{
			StartCrawl();
		}
		else if (bIsCrawling && (!bWantsToCrawling || !CanCrawlingInCurrentState()))
		{
			StopCrawl();
		}
		//need state = crouch is true
		if (bWasJumpWithCrawling && !IsCrawling())
		{
			CachedBaseCharacter->bIsCrouched = true;
		}
	}
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
	if (UpdatedComponent && CachedBaseCharacter)
	{
		const bool bIsSprinting = IsSprinting();
		if (!bIsSprinting && bWantsToSprint && CanSprintInCurrentState())
		{
			//setup speed to sprint
			ChangeSprint(true);
		}
		else if (bIsSprinting && (!bWantsToSprint || !CanSprintInCurrentState()))
		{
			//default speed
			ChangeSprint(false);
		}
	}
}

void UOBaseCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);
	if (UpdatedComponent && CachedBaseCharacter)
	{
		const bool bIsSprinting = IsSprinting();
		if (bIsSprinting && (Velocity.IsNearlyZero() || !CanSprintInCurrentState()))
		{
			ChangeSprint(false);
		}
	}
	
}

float UOBaseCharacterMovementComponent::GetMaxSpeed() const
{
	if (MovementMode == MOVE_Walking || MovementMode == MOVE_NavWalking)
	{
		if (IsCrouching())
		{
			return IsSprinting() ? SprintingMaxSpeedCrouch : MaxWalkSpeedCrouched;
		}
		if (IsOutOfStamina())
		{
			return MaxSpeedOutOfStamina;
		}
		if (IsCrawling())
		{
			return MaxCrawlSpeed;
		}
		return IsSprinting() ? SprintingMaxSpeedWalking : MaxWalkSpeed;
	}
	if (MovementMode == MOVE_Swimming)
	{
		float CoefSpeed = CachedBaseCharacter && CachedBaseCharacter->GetIsOverlapVolumeSurface() ? CoeffSpeedSwimmingOnSurface : 1.f;
		return IsSprinting() ? SprintingMaxSpeedSwimming * CoefSpeed : MaxSwimSpeed * CoefSpeed;
	}
	if (MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_ClimbLadder)
	{
		return IsSprinting() ? SprintingClimbingLadderMaxSpeed : ClimbingLadderMaxSpeed;
	}
	return Super::GetMaxSpeed();
}

bool UOBaseCharacterMovementComponent::IsOutOfStamina() const
{
	return CachedBaseCharacter && CachedBaseCharacter->GetIsOutOfStamina();
}

bool UOBaseCharacterMovementComponent::IsCrawling() const
{
	return CachedBaseCharacter && CachedBaseCharacter->GetIsCrawling();
}

bool UOBaseCharacterMovementComponent::CanCrawlingInCurrentState() const
{
	return (IsFalling() || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

void UOBaseCharacterMovementComponent::StartCrawl()
{
	if (!HasValidData())
	{
		return;
	}

	if (!CanCrawlingInCurrentState())
	{
		return;
	}

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == CrawlingHalfHeight)
	{
		CachedBaseCharacter->SetIsCrawling(true);
		CachedBaseCharacter->OnStartCrawling( 0.f, 0.f );
		return;
	}

	// Change collision size to Crawling dimensions
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	// Height is not allowed to be smaller than radius.
	const float ClampedCrawlingHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, CrawlingHalfHeight);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedCrawlingHalfHeight);
	const float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedCrawlingHalfHeight);
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	//block for client
	// Crawling to a larger height? (this is rare)
	if (ClampedCrawlingHalfHeight > OldUnscaledHalfHeight)
	{
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);
		const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust), FQuat::Identity,UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

		// If encroached, cancel
		if (bEncroached)
		{
			CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);
			return;
		}
	}
	if (bCrouchMaintainsBaseLocation)
	{
		// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
		UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	}
	CachedBaseCharacter->SetIsCrawling(true);

	bForceNextFloorCheck = true;
	CachedBaseCharacter->OnStartCrawling(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UOBaseCharacterMovementComponent::StopCrawl()
{
	if (!HasValidData())
	{
		return;
	}	
	// See if collision is already at desired size.
	// Change collision size to crouching dimensions
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	// Height is not allowed to be smaller than radius.
	const float ClampedCrouchedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, CrouchedHalfHeight);
	//if MOVE_Swimming == MovementMode size capsule change to swim
	if (MOVE_Swimming == MovementMode || CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == ClampedCrouchedHalfHeight)
	{
		CachedBaseCharacter->SetIsCrawling(false);
		CachedBaseCharacter->OnEndCrawling( 0.f, 0.f);
		return;
	}
	const float CurrentCrawlingHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float HalfHeightAdjust = ClampedCrouchedHalfHeight - CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
	
	// Grow to uncrouched size.
	check(CharacterOwner->GetCapsuleComponent());

	// Try to stay in place and see if the larger cPawnLocation = {FVector} apsule fits. We use a slightly taller capsule to avoid penetration.
	const UWorld* MyWorld = GetWorld();
	const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);

	// Compensate for the difference between current capsule size and standing size
	const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust); // Shrink by negative amount, so actually grow it.
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	bool bEncroached = true;

	// Expand while keeping base location the same.
	FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentCrawlingHalfHeight);
	bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

	if (bEncroached)
	{
		if (IsMovingOnGround())
		{
			// Something might be just barely overhead, try moving down closer to the floor to avoid it.
			const float MinFloorDist = KINDA_SMALL_NUMBER * 10.f;
			if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
			{
				StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
				bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
			}
		}				
	}

	if (!bEncroached)
	{
		// Commit the change in location.
		UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		bForceNextFloorCheck = true;
	}

	// If still encroached then abort.
	if (bEncroached)
	{
		return;
	}
	CachedBaseCharacter->SetIsCrawling(false);

	// Now call SetCapsuleSize() to cause touch/untouch events and actually grow the capsule
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedCrouchedHalfHeight, true);
	CachedBaseCharacter->OnEndCrawling(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UOBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingMovementParameters)
{
	CurrentMantlingParameters = MantlingMovementParameters;
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
}

void UOBaseCharacterMovementComponent::StopMantle()
{
	SetMovementMode(EMovementMode::MOVE_Walking);
}

bool UOBaseCharacterMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling;
}

void UOBaseCharacterMovementComponent::AttachLadder(const AOLadderInteractiveActor* Ladder)
{
	if (IsValid(CachedBaseCharacter))
	{
		CachedBaseCharacter->bUseControllerRotationYaw = false;
	}
	CurrentLadder = Ladder;
	bCrossLadderMinBottomOffset = false;
	FRotator TargetOrientationRotation = CurrentLadder->GetActorRotation();
	GEngine->AddOnScreenDebugMessage(66, 10.0f, FColor::Yellow, FString::Format(TEXT(" IK loc {0} --- {1} -- {2} "), {TargetOrientationRotation.ToString(), CurrentLadder->GetActorForwardVector().ToOrientationRotator().ToString(), GetOwner()->GetActorForwardVector().ToOrientationRotator().ToString()}), true);
	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	const FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();

	const FVector NewLocation = CurrentLadder->GetActorLocation() + GetProjectionFromActorToLadder(GetActorLocation()) * LadderUpVector + LadderToCharacterOffset * LadderForwardVector;
	
	GetOwner()->SetActorLocation(NewLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotation);
	GetOwner()->AddActorLocalRotation(FRotator(0.f, 180, 0.f));

	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_ClimbLadder);
}

float UOBaseCharacterMovementComponent::GetProjectionFromActorToLadder(const FVector& CurrentLocation) const
{
	if (CurrentLadder == nullptr)
	{
		return 0.f;
	}
	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	const FVector LadderToCharacterDistance = CurrentLocation - CurrentLadder->GetActorLocation();
	return FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}

void UOBaseCharacterMovementComponent::DetachFromLadder(bool bWithJump)
{
	if (IsValid(CachedBaseCharacter))
	{
		CachedBaseCharacter->bUseControllerRotationYaw = true;
	}
	CurrentLadder = nullptr;
	SetMovementMode(bWithJump ? EMovementMode::MOVE_Falling : EMovementMode::MOVE_Walking);
}

bool UOBaseCharacterMovementComponent::IsClimbingLadder() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_ClimbLadder;
}

bool UOBaseCharacterMovementComponent::ShouldRemainVertical() const
{
	return Super::ShouldRemainVertical() || (CachedBaseCharacter && CachedBaseCharacter->GetIsOverlapVolumeSurface());
}

void UOBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (IsClimbingLadder())
	{
		return;
	}
	Super::PhysicsRotation(DeltaTime);
}

void UOBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (MovementMode == MOVE_Swimming)
	{
		if (CachedBaseCharacter)
		{
			CachedBaseCharacter->UnCrouch();
			CachedBaseCharacter->UnCrawl();			
		}
		const float ClampedSwimmingHalfHeight = FMath::Max3(0.f, SwimmingRadiusSize, SwimmingCapsuleHalfSize);
		const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
		const float HalfHeightAdjust = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ClampedSwimmingHalfHeight;
		const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimmingRadiusSize, ClampedSwimmingHalfHeight, true);
		CachedBaseCharacter->OnStartSwimming(HalfHeightAdjust, ScaledHalfHeightAdjust);
	}
	else if (PreviousMovementMode == MOVE_Swimming)
	{
		const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
  		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);
		CachedBaseCharacter->OnEndSwimming(HalfHeightAdjust, ScaledHalfHeightAdjust);
	}
	if (PreviousMovementMode == MOVE_Custom)
	{
		switch (PreviousCustomMode)
		{
			case (uint8)ECustomMovementMode::CMOVE_ClimbLadder:
			{
				GetOwner()->SetActorRotation(FRotator(0, GetOwner()->GetActorRotation().Yaw, 0));
				break;
			}

			default:
			{
				break;
			}
		}
	}

	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
			case (uint8)ECustomMovementMode::CMOVE_Mantling:
			{
				GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UOBaseCharacterMovementComponent::StopMantle, CurrentMantlingParameters.Duration, false);
				break;
			}

			default:
			{
				break;
			}
		}
	}
}

void UOBaseCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
		case (uint8)ECustomMovementMode::CMOVE_Mantling:
		{
			PhysMantling(DeltaTime, Iterations);
			break;
		}
		case (uint8)ECustomMovementMode::CMOVE_ClimbLadder:
		{
			PhysClimbLadder(DeltaTime, Iterations);
			break;
		}

		default:
		{
			break;
		}
	}

	
	Super::PhysCustom(DeltaTime, Iterations);
}

void UOBaseCharacterMovementComponent::UnCrouch(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}
	//if MOVE_Swimming == MovementMode size capsule change to swim
	if (MOVE_Swimming == MovementMode || CachedBaseCharacter && CachedBaseCharacter->GetIsCrawling())
	{
		CachedBaseCharacter->bIsCrouched = false;		
		CachedBaseCharacter->OnEndCrouch(0.f, 0.f);
		return;
	}
	Super::UnCrouch(bClientSimulation);
}

void UOBaseCharacterMovementComponent::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);
}

void UOBaseCharacterMovementComponent::PhysMantling(float DeltaTime, int32 Iterations)
{
	const float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParameters.StartTime;
	const FVector MantlingCurveVector = CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);
	const float PositionAlpha = MantlingCurveVector.X;
	const float XYCorrectionAlpha = MantlingCurveVector.Y;
	const float ZCorrectionAlpha = MantlingCurveVector.Z;

	FVector CorrectorInitialLocation = FMath::Lerp(CurrentMantlingParameters.InitialLocation, CurrentMantlingParameters.InitialAnimationLocation, XYCorrectionAlpha);
	CorrectorInitialLocation.Z = FMath::Lerp(CurrentMantlingParameters.InitialLocation.Z, CurrentMantlingParameters.InitialAnimationLocation.Z, ZCorrectionAlpha);

	const FVector NewLocation = FMath::Lerp(CorrectorInitialLocation, CurrentMantlingParameters.TargetLocation, PositionAlpha);
	const FRotator NewRotation = FMath::Lerp(CurrentMantlingParameters.InitialRotation, CurrentMantlingParameters.TargetRotation, PositionAlpha);

	const FVector Delta = NewLocation - GetActorLocation();

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit);
}

void UOBaseCharacterMovementComponent::PhysClimbLadder(float DeltaTime, int32 Iterations)
{
	CalcVelocity(DeltaTime, 1.f, false, ClimbingLadderBrakingDeceleration);
	const FVector Delta = Velocity * DeltaTime;

	const FVector NewLocation = GetActorLocation() + Delta;
	float NewPosProjection = GetProjectionFromActorToLadder(NewLocation);

	if (!bCrossLadderMinBottomOffset && NewPosProjection > LadderMinBottomOffset)
	{
		bCrossLadderMinBottomOffset = true;
	}
	if (bCrossLadderMinBottomOffset && NewPosProjection < LadderMinBottomOffset)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Format(TEXT(" Ladder Detach {0} {1} {2} "), {NewPosProjection , NewPosProjection + NewLocation.Z,  CurrentLadder->GetActorLocation().Z + LadderMinBottomOffset}));
		DetachFromLadder(false);
		return;
	}
	if (NewPosProjection > CurrentLadder->GetHeight() - LadderMaxTopffset)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, FString::Format(TEXT(" Ladder !!! {0} { 1} {2} "), {NewPosProjection + NewLocation.Z , CurrentLadder->GetActorLocation().Z + CurrentLadder->GetHeight(),  CurrentLadder->GetActorLocation().Z + CurrentLadder->GetHeight() - LadderMaxTopffset}));
		CachedBaseCharacter->Mantle();
		return;
	}
	
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}

const AOLadderInteractiveActor* UOBaseCharacterMovementComponent::GetCurrentLadder() const
{
	return CurrentLadder;
}

float UOBaseCharacterMovementComponent::GetClimbingLadderSpeedRatio() const
{
	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingLadderMaxSpeed;
}

float UOBaseCharacterMovementComponent::GetLadderToCharacterOffset() const
{
	return LadderToCharacterOffset;
}
