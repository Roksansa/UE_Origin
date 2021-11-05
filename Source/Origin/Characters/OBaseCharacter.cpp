// Fill out your copyright notice in the Description page of Project Settings.


#include "OBaseCharacter.h"

#include "Actors/Weapon/OBaseWeapon.h"
#include "Components/OCharacterIKComponent.h"
#include "Components/OWeaponComponent.h"
#include "Curves/CurveVector.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Origin/Actors/OInteractiveActor.h"
#include "Origin/Actors/OLadderInteractiveActor.h"
#include "Origin/Components/OLedgeDetectorComponent.h"
#include "Origin/MovementComponents/OBaseCharacterMovementComponent.h"

AOBaseCharacter::AOBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	if (ensureAlwaysMsgf(GetCharacterMovement()->IsA<UOBaseCharacterMovementComponent>(),
		TEXT("AOBaseCharacter::AOBaseCharacter BaseMovementComponent work only UOBaseCharacterMovementComponent and children")))
	{
		BaseCharacterMovementComponent = StaticCast<UOBaseCharacterMovementComponent*>(GetCharacterMovement());
	}

	LedgeDetectorComponent = CreateDefaultSubobject<UOLedgeDetectorComponent>(TEXT("LedgeDetector"));
	IKComponent = CreateDefaultSubobject<UOCharacterIKComponent>(TEXT("CharacterIKComponent"));
	WeaponComponent = CreateDefaultSubobject<UOWeaponComponent>(TEXT("WeaponComponent"));
}

void AOBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentStamina = MaxStamina;
	if (MinStaminaForStartSprint > MaxStamina)
	{
		MinStaminaForStartSprint = MaxStamina;
	}
}

void AOBaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TryChangeStamina(DeltaSeconds);
}

bool AOBaseCharacter::GetIsOutOfStamina() const
{
	return bIsOutOfStamina;
}

bool AOBaseCharacter::GetIsCrawling() const
{
	return bIsCrawling == 1;
}

void AOBaseCharacter::SetIsCrawling(bool bCrawl)
{
	bIsCrawling = bCrawl ? 1 : 0;
}

void AOBaseCharacter::UnCrawl(bool bClientSimulation)
{
	if (BaseCharacterMovementComponent)
	{
		BaseCharacterMovementComponent->bWantsToCrawling = false;
	}
}

void AOBaseCharacter::Crawl(bool bClientSimulation)
{
	if (BaseCharacterMovementComponent)
	{
		BaseCharacterMovementComponent->bWantsToCrawling = true;
	}
}

void AOBaseCharacter::ChangeCrouchState()
{
	if (!(GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()))
	{
		return;
	}
	if (BaseCharacterMovementComponent && BaseCharacterMovementComponent->bWantsToCrawling || GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AOBaseCharacter::ChangeCrawlState()
{
	if (!(GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()))
	{
		return;
	}
	if (BaseCharacterMovementComponent && GetBaseCharacterMovementComponent()->IsCrawling())
	{
		UnCrawl();
	}
	else if (GetCharacterMovement()->IsCrouching())
	{
		Crawl();
	}
}

UOBaseCharacterMovementComponent* AOBaseCharacter::GetBaseCharacterMovementComponent() const
{
	return BaseCharacterMovementComponent;
}

UOCharacterIKComponent* AOBaseCharacter::GetIKComponent() const
{
	return IKComponent;
}

bool AOBaseCharacter::CanSprint()
{
	if (BaseCharacterMovementComponent)
	{
		return !bIsOutOfStamina && !bIsCrawling && BaseCharacterMovementComponent->CanSprint();
	}

	return !bIsOutOfStamina;
}

void AOBaseCharacter::TryChangeStamina(float DeltaSeconds)
{
	//check if !bIsSprinting
	if (!bIsSprinting && CurrentStamina < MaxStamina)
	{
		CurrentStamina = FMath::Min(CurrentStamina + StaminaRestoreVelocity * DeltaSeconds, MaxStamina);
	}
	//check if bIsSprinting
	if (bIsSprinting && CurrentStamina > 0.0001f)
	{
		CurrentStamina = FMath::Max(CurrentStamina - SprintStaminaConsumptionVelocity * DeltaSeconds, 0.f);
	}

	//switch IsOutOfStamina after all calc
	//first - we can not start sprint - change and return
	if (!bIsOutOfStamina && CurrentStamina <= 0.0001f)
	{
		bIsOutOfStamina = true;
	}
	//second - we can start sprint - change and return
	if (bIsOutOfStamina && CurrentStamina >= MinStaminaForStartSprint)
	{
		bIsOutOfStamina = false;
	}

	if (CurrentStamina < MaxStamina)
	{
		const FColor CurrentColor = bIsOutOfStamina ? FColor::Red : FColor::Green;
		GEngine->AddOnScreenDebugMessage(1, 1.0f, CurrentColor, FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina), true,
			FVector2D(2, 2));
	}
}

void AOBaseCharacter::ChangeSprint(bool bWantsToSprint)
{
	if (BaseCharacterMovementComponent)
	{
		if (bWantsToSprint)
		{
			if (CanSprint())
			{
				BaseCharacterMovementComponent->bWantsToSprint = true;
			}
		}
		else
		{
			BaseCharacterMovementComponent->bWantsToSprint = false;
		}
	}
}

void AOBaseCharacter::Jump()
{
	if (BaseCharacterMovementComponent && BaseCharacterMovementComponent->IsMantling())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green,
			FString::Format(TEXT(" JumpMantle CHECK {0} "), {BaseCharacterMovementComponent->IsMantling()}));
		return;
	}
	if (bIsCrawling && BaseCharacterMovementComponent)
	{
		BaseCharacterMovementComponent->bWantsToCrawling = false;
		BaseCharacterMovementComponent->bWantsToCrouch = false;
		return;
	}
	if (bIsCrouched)
	{
		GetCharacterMovement()->bWantsToCrouch = false;
	}
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Format(TEXT("Jump Mantle {0} "), {false}));
	Super::Jump();
}

void AOBaseCharacter::OnStartCrawling(float HalfHeightAdjust, float ScaledHeightAdjust)
{
}

void AOBaseCharacter::OnEndCrawling(float HalfHeightAdjust, float ScaledHeightAdjust)
{
}

void AOBaseCharacter::OnStartSwimming(float HalfHeightAdjust, float ScaledHeightAdjust)
{
}

void AOBaseCharacter::OnEndSwimming(float HalfHeightAdjust, float ScaledHeightAdjust)
{
}

const AOLadderInteractiveActor* AOBaseCharacter::GetAvailableLadder() const
{
	const AOLadderInteractiveActor* Result = nullptr;
	for (const AOInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<AOLadderInteractiveActor>())
		{
			Result = StaticCast<const AOLadderInteractiveActor*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

void AOBaseCharacter::FillMantlingMovementParameters(FLedgeDescription LedgeDescription,
	FMantlingMovementParameters& MantlingMovementParameters) const
{
	MantlingMovementParameters.InitialLocation = GetActorLocation();
	MantlingMovementParameters.InitialRotation = GetActorRotation();
	MantlingMovementParameters.TargetLocation = LedgeDescription.Location;
	MantlingMovementParameters.TargetRotation = LedgeDescription.Rotation;

	const float MantlingHeight = (MantlingMovementParameters.TargetLocation - MantlingMovementParameters.InitialLocation).Z;
	const FOMantlingSettings CurrentMantlingSettings = GetMantlingSettings(MantlingHeight);

	float MaxRange;
	float MinRange;
	CurrentMantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);
	const FVector2D SourceRange{CurrentMantlingSettings.MinHeight, CurrentMantlingSettings.MaxHeight};
	const FVector2D TimeRange{CurrentMantlingSettings.MinHeightStartTime, CurrentMantlingSettings.MaxHeightStartTime};
	MantlingMovementParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TimeRange, MantlingHeight);
	MantlingMovementParameters.Duration = MaxRange - MantlingMovementParameters.StartTime;
	MantlingMovementParameters.MantlingCurve = CurrentMantlingSettings.MantlingCurve;
	MantlingMovementParameters.InitialAnimationLocation =
		MantlingMovementParameters.TargetLocation - CurrentMantlingSettings.AnimationCorrectionZ * FVector::UpVector +
		CurrentMantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;
}

const FOMantlingSettings& AOBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight < LowMantlingSettings.MaxHeight ? LowMantlingSettings : HighMantlingSettings;
}

void AOBaseCharacter::Mantle()
{
	if (BaseCharacterMovementComponent && BaseCharacterMovementComponent->IsMantling() || bIsCrawling)
	{
		return;
	}
	FLedgeDescription LedgeDescription;
	if (LedgeDetectorComponent->TryDetectLedge(LedgeDescription))
	{
		FMantlingMovementParameters MantlingMovementParameters;
		FillMantlingMovementParameters(LedgeDescription, MantlingMovementParameters);

		BaseCharacterMovementComponent->StartMantle(MantlingMovementParameters);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		const float MantlingHeight = (MantlingMovementParameters.TargetLocation - MantlingMovementParameters.InitialLocation).Z;
		const FOMantlingSettings CurrentMantlingSettings = GetMantlingSettings(MantlingHeight);
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Format(TEXT("Mantle {0} "), {MantlingHeight}));
		AnimInstance->Montage_Play(CurrentMantlingSettings.Montage, 1.0f, EMontagePlayReturnType::Duration,
			MantlingMovementParameters.StartTime);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Format(TEXT("Mantle {0} "), {false}));
	}
}

bool AOBaseCharacter::GetIsOverlapVolumeSurface() const
{
	return bIsOverlapVolumeSurface;
}

void AOBaseCharacter::ChangeBuoyancyFromSurfaceVolume(bool bIsOverlapVolumeSurfaceNow)
{
	bIsOverlapVolumeSurface = bIsOverlapVolumeSurfaceNow;
	if (GetCharacterMovement())
	{
		ACharacter* DefaultCharacter = GetClass()->GetDefaultObject<ACharacter>();
		GetCharacterMovement()->Buoyancy = bIsOverlapVolumeSurface
			                                   ? BuoyancyInWaterSurface
			                                   : DefaultCharacter->GetCharacterMovement()->Buoyancy;
	}
}

void AOBaseCharacter::RegisterInteractiveActor(AOInteractiveActor* InterActor)
{
	AvailableInteractiveActors.Add(InterActor);
}

void AOBaseCharacter::UnregisterInteractiveActor(AOInteractiveActor* InterActor)
{
	AvailableInteractiveActors.Remove(InterActor);
}
