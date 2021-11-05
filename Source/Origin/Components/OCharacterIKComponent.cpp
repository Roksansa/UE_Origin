// Fill out your copyright notice in the Description page of Project Settings.


#include "OCharacterIKComponent.h"

#include "DrawDebugHelpers.h"
#include "OTypes.h"
#include "Actors/OLadderInteractiveActor.h"
#include "Characters/OBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MovementComponents/OBaseCharacterMovementComponent.h"
#include "Subsystems/ODebugSubsystem.h"

UOCharacterIKComponent::UOCharacterIKComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UOCharacterIKComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetOwner()->IsA<AOBaseCharacter>(), TEXT("IK Component works only AOBaseCharacter or his children"))
	CachedCharacter = StaticCast<AOBaseCharacter*>(GetOwner());
	CachedMovComp = CachedCharacter->GetBaseCharacterMovementComponent();

	IKScale = CachedCharacter->GetActorScale3D().Z;
	IKLegLiftRealDistance = (IKLegLiftDistance) * IKScale;
	IKLegBeyondCapsuleRealDistance = IKLegBeyondCapsuleDistance * IKScale;
	IKTraceRealDistance = IKLegLiftRealDistance + IKLegBeyondCapsuleRealDistance;
}


float UOCharacterIKComponent::GetIKOffsetForSocketName(const FName& SocketName) const
{
	float Result = 0.f;

	if (!CachedCharacter.IsValid())
	{
		return Result;
	}

	const FVector SocketLocation = CachedCharacter->GetMesh()->GetSocketLocation(SocketName);
	//GetActorLocation().Z  - position capsule component on middle capsule. = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + position floor;
	const float IKLegLiftRealDistanceWithOrWithoutCrouch = IKLegLiftRealDistance / (!CachedCharacter->bIsCrouched ? 1 : 2);
	const FVector TraceStart(SocketLocation.X, SocketLocation.Y,
		CachedCharacter->GetActorLocation().Z - CachedCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() +
		IKLegLiftRealDistanceWithOrWithoutCrouch);
	//max distance = distance to floor + leg lift distance
	const FVector TraceEnd = TraceStart - IKTraceRealDistance * FVector::UpVector;

	FHitResult HitResult;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	EDrawDebugTrace::Type EDebugTrace = EDrawDebugTrace::None;
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	UODebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UODebugSubsystem>();
	EDebugTrace = DebugSubsystem->IsCategoryEnabled(DebugCategory_IKFootCharacter) ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
#endif
	TArray<AActor*> IgnoreActors;
	const FRotator Orientation = CachedCharacter->GetMesh()->GetSocketRotation(SocketName) + FRotator(0, 90, 90);
	if (UKismetSystemLibrary::BoxTraceSingle(GetOwner(), TraceStart, TraceEnd, HalfSizeBoxForSweep, Orientation, TraceType, true,
		IgnoreActors, EDebugTrace, HitResult, true))
	{
		//TraceStart.Z - IKLegLiftRealDistance =  floor point
		Result = (TraceStart.Z - IKLegLiftRealDistanceWithOrWithoutCrouch - HitResult.Location.Z - (
			          !CachedCharacter->bIsCrouched && !CachedCharacter->GetIsOutOfStamina() ? IKFootCorrection * IKScale : 0.f)) / IKScale;
	}

	return Result;
}

FVector UOCharacterIKComponent::GetIKOffsetFromLadderStepForSocketName(const FName& SocketName) const
{
	FVector Result = FVector::ZeroVector;

	if (!CachedCharacter.IsValid() || !CachedMovComp.IsValid())
	{
		return Result;
	}

	const float StartOffset = 72.f;
	const FVector Start = CachedCharacter->GetActorLocation();

	FVector TraceStart(Start.X, Start.Y, Start.Z);
	const FRotator ActorRotation = CachedCharacter->GetActorRotation();
	TraceStart += ActorRotation.RotateVector(
		(-CachedCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + StartOffset) * FVector::UpVector);
	TraceStart += ActorRotation.RotateVector((CachedMovComp->GetLadderToCharacterOffset()) * FVector::ForwardVector);
	const float TraceRealDistance = CachedMovComp->GetCurrentLadder()->GetStepsInterval();

	const FVector TraceEnd = TraceStart - TraceRealDistance * CachedCharacter->GetActorUpVector();
	DrawDebugPoint(GetWorld(), TraceEnd, 20.f, FColor::Yellow, false, 2.f);

	FHitResult HitResult;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_InteractionObjectTrace);

	EDrawDebugTrace::Type EDebugTrace = EDrawDebugTrace::None;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	UODebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UODebugSubsystem>();
	EDebugTrace = DebugSubsystem->IsCategoryEnabled(DebugCategory_IKFootCharacter) ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
#endif
	TArray<AActor*> IgnoreActors;
	const FVector SocketLocation = CachedCharacter->GetMesh()->GetSocketLocation(SocketName);
	DrawDebugPoint(GetWorld(), SocketLocation, 10.f, FColor::Black, false, 2.f);
	if (UKismetSystemLibrary::BoxTraceSingle(GetOwner(), TraceStart, TraceEnd, HalfSizeBoxForSweepLadder, ActorRotation, TraceType,
		true, IgnoreActors, EDebugTrace, HitResult, true))
	{
		Result = (TraceStart - HitResult.ImpactPoint) / IKScale;
		DrawDebugPoint(GetWorld(), Result, 10.f, FColor::Magenta, false, 2.f);
	}
	else
	{
		Result = ActorRotation.RotateVector(CachedCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * FVector::UpVector);
	}

	if (Result.Z > MinFootOffsetResultZ)
	{
		const float DiffX = MaxFootOffsetResultX - MinFootOffsetResultX;
		const float DiffZ = MaxFootOffsetResultZ - MinFootOffsetResultZ;
		//offset for feet closer than default offset between feet
		//calc as compare result.Z = 30 == MinOffsetX = 20.f and result.Z = 60 == MaxOffsetX = 40; 
		float CurrentOffset = (Result.Z - MinFootOffsetResultZ) * (DiffX / DiffZ) + MinFootOffsetResultX;
		CurrentOffset = FMath::Clamp(CurrentOffset, MinFootOffsetResultX, MaxFootOffsetResultX);

		Result -= ActorRotation.RotateVector(CurrentOffset * FVector::ForwardVector);
	}
	Result = ActorRotation.UnrotateVector(Result);
	return Result;
}

FVector UOCharacterIKComponent::GetIKHandOffsetFromLadderStepForSocketName(const FName& SocketName) const
{
	FVector Result = FVector::ZeroVector;

	if (!CachedCharacter.IsValid() || !CachedMovComp.IsValid())
	{
		return Result;
	}

	const FVector HandOffset = SocketName == RightHandSocketName ? FVector(5, 0, 5.f) : FVector(5, 0, 5.f);
	const float StepsInterval = CachedMovComp->GetCurrentLadder()->GetStepsInterval();
	//magic offset for hand Z
	const float HandStartOffset = StepsInterval > 55.f ? 35.f : 20.f;

	const FVector Start = CachedCharacter->GetActorLocation();
	FVector TraceStart(Start.X, Start.Y, Start.Z);
	const FRotator ActorRotation = CachedCharacter->GetActorRotation();
	const float HalfHeight = CachedCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	TraceStart += ActorRotation.RotateVector((HalfHeight * 0.5f + StepsInterval - HandStartOffset) * FVector::UpVector);
	TraceStart += ActorRotation.RotateVector((CachedMovComp->GetLadderToCharacterOffset()) * FVector::ForwardVector);
	DrawDebugPoint(GetWorld(), TraceStart, 10.f, FColor::Magenta, false, 2.f);
	const float TraceRealDistance = StepsInterval;

	const FVector TraceEnd = TraceStart - TraceRealDistance * CachedCharacter->GetActorUpVector();

	FHitResult HitResult;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_InteractionObjectTrace);

	EDrawDebugTrace::Type EDebugTrace = EDrawDebugTrace::None;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	UODebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UODebugSubsystem>();
	EDebugTrace = DebugSubsystem->IsCategoryEnabled(DebugCategory_IKFootCharacter) ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
#endif
	TArray<AActor*> IgnoreActors;
	if (UKismetSystemLibrary::BoxTraceSingle(GetOwner(), TraceStart, TraceEnd, HalfSizeBoxForSweepLadder, ActorRotation, TraceType, true,
		IgnoreActors, EDebugTrace, HitResult, true))
	{
		Result = (TraceStart - HitResult.ImpactPoint) / IKScale;
		Result -= ActorRotation.RotateVector((StepsInterval - HandStartOffset) * FVector::UpVector);
		Result += ActorRotation.RotateVector(-HandOffset * (Result.Z < 0 ? 1.7f : 1) / IKScale);
	}
	Result = ActorRotation.UnrotateVector(Result);
	return Result;
}

FName UOCharacterIKComponent::GetRightFootSocketName() const
{
	return RightFootSocketName;
}

FName UOCharacterIKComponent::GetLeftFootSocketName() const
{
	return LeftFootSocketName;
}

FName UOCharacterIKComponent::GetRightHandSocketName() const
{
	return RightHandSocketName;
}

FName UOCharacterIKComponent::GetLeftHandSocketName() const
{
	return LeftHandSocketName;
}

float UOCharacterIKComponent::GetIKInterpSpeed() const
{
	return IKInterpSpeed;
}
