// Fill out your copyright notice in the Description page of Project Settings.


#include "OBaseCharacter.h"

#include "Actors/Weapon/OBaseWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/OCharacterIKComponent.h"
#include "Components/OPrimaryAttributesComponent.h"
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
	PrimaryAttributesComponent = CreateDefaultSubobject<UOPrimaryAttributesComponent>(TEXT("AttributesComponent"));
}

void AOBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	check(PrimaryAttributesComponent);
	check(BaseCharacterMovementComponent);
	check(WeaponComponent);

	PrimaryAttributesComponent->OnDie.AddUObject(this, &AOBaseCharacter::OnDie);
	PrimaryAttributesComponent->OnChangeHealth.AddUObject(this, &AOBaseCharacter::OnChangeHealth);
}

void AOBaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	PrimaryAttributesComponent->TryChangeStamina(DeltaSeconds, bIsSprinting);
}

bool AOBaseCharacter::GetIsOutOfStamina() const
{
	return PrimaryAttributesComponent->GetIsOutOfStamina();
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
	BaseCharacterMovementComponent->bWantsToCrawling = false;
}

void AOBaseCharacter::Crawl(bool bClientSimulation)
{
	BaseCharacterMovementComponent->bWantsToCrawling = true;
}

void AOBaseCharacter::ChangeCrouchState()
{
	if (!(GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()))
	{
		return;
	}
	if (BaseCharacterMovementComponent->bWantsToCrawling || GetCharacterMovement()->IsCrouching())
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
	if (BaseCharacterMovementComponent->IsCrawling())
	{
		UnCrawl();
	}
	else if (BaseCharacterMovementComponent->IsCrouching())
	{
		Crawl();
	}
}

void AOBaseCharacter::StartFire()
{
	WeaponComponent->StartFire();
}

void AOBaseCharacter::StopFire()
{
	WeaponComponent->StopFire();
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
	return !PrimaryAttributesComponent->GetIsOutOfStamina() && !bIsCrawling && BaseCharacterMovementComponent->CanSprint();
}


void AOBaseCharacter::ChangeSprint(bool bWantsToSprint)
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

void AOBaseCharacter::Jump()
{
	if (BaseCharacterMovementComponent->IsMantling())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green,
			FString::Format(TEXT(" JumpMantle CHECK {0} "), {BaseCharacterMovementComponent->IsMantling()}));
		return;
	}
	if (bIsCrawling)
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
	if (BaseCharacterMovementComponent->IsMantling() || bIsCrawling)
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

void AOBaseCharacter::NextWeapon()
{
	WeaponComponent->NextWeapon();
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

const UOWeaponComponent* AOBaseCharacter::GetWeaponComponent() const
{
	return WeaponComponent;
}

void AOBaseCharacter::OnDie()
{
	PlayAnimMontage(DeathAnimMontage);
	GetCharacterMovement()->DisableMovement();
	GetController()->DisableInput(nullptr);
	SetLifeSpan(5.f);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponComponent->StopFire();
}

void AOBaseCharacter::OnChangeHealth(float Health, float Diff)
{
	if (Health > SMALL_NUMBER && Diff < 0)
	{
		const FOWeaponAnimDescription Desc = WeaponComponent->GetWeaponAnimDescription();
		PlayAnimMontage(Desc.HitAnimMontage);
	}
}
