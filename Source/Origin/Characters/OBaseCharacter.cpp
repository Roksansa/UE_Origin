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

void AOBaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(CheckAimingTimerHandle);
	Super::EndPlay(EndPlayReason);
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

bool AOBaseCharacter::CanUseWeapon() const
{
	const bool bWalkingMode = BaseCharacterMovementComponent->MovementMode == EMovementMode::MOVE_Walking || BaseCharacterMovementComponent->MovementMode ==
	                          EMovementMode::MOVE_NavWalking;
	if (PrimaryAttributesComponent->GetIsOutOfStamina() || bIsSprinting || !bWalkingMode)
	{
		return false;
	}
	return true;
}

void AOBaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (BaseCharacterMovementComponent->MovementMode == MOVE_Swimming)
	{
		WeaponComponent->NextWeapon(1);
	}
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}

void AOBaseCharacter::InteractionWithLadder()
{
	if(BaseCharacterMovementComponent->IsMantling())
	{
		return;
	}
	
	if (BaseCharacterMovementComponent->IsClimbingLadder())
	{
		BaseCharacterMovementComponent->DetachFromLadder(true);
	}
	else
	{
		const AOLadderInteractiveActor* Ladder = GetAvailableLadder();
		if (!IsValid(Ladder))
		{
			return;
		}
		BaseCharacterMovementComponent->AttachLadder(Ladder);
	}
}

void AOBaseCharacter::StartFire()
{
	if (!CanUseWeapon())
	{
		return;
	}
	WeaponComponent->StartFire();
}

void AOBaseCharacter::StopFire()
{
	WeaponComponent->StopFire();
}

void AOBaseCharacter::ReloadAmmo()
{
	if (!CanUseWeapon())
	{
		return;
	}
	WeaponComponent->ReloadAmmo();
}

void AOBaseCharacter::NextWeapon()
{
	if (!CanUseWeapon())
	{
		return;
	}
	WeaponComponent->NextWeapon();
}

void AOBaseCharacter::NextWeaponIndex(int32 NumberWeapon)
{
	if (!CanUseWeapon())
	{
		return;
	}
	WeaponComponent->NextWeapon(NumberWeapon);
}

void AOBaseCharacter::StartAiming()
{
	if (WeaponComponent->GetWeaponType() == EOEquippableItemType::None || !CanUseWeapon())
	{
		return;
	}

	bWantAiming = true;
	OnChangeAiming.Broadcast(bWantAiming);
	GetWorld()->GetTimerManager().ClearTimer(CheckAimingTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(CheckAimingTimerHandle, [this]()
	{
		const bool bAllowAiming = CanUseWeapon();

		if (bAllowAiming && bWantAiming && !WeaponComponent->GetAiming() && WeaponComponent->GetState() != EOWeaponUseState::None)
		{
			OnChangeAiming.Broadcast(bAllowAiming);
			return;
		}

		if (!bAllowAiming && bWantAiming && WeaponComponent->GetAiming() && WeaponComponent->GetState() != EOWeaponUseState::None)
		{
			OnChangeAiming.Broadcast(bAllowAiming);
		}
	}, 0.5f, true);
}

void AOBaseCharacter::StopAiming()
{
	if (bWantAiming)
	{
		bWantAiming = false;
		OnChangeAiming.Broadcast(bWantAiming);
	}
	GetWorld()->GetTimerManager().ClearTimer(CheckAimingTimerHandle);
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

void AOBaseCharacter::FillMantlingMovementParameters(FOLedgeDescription LedgeDescription,
	FOMantlingMovementParameters& MantlingMovementParameters) const
{
	MantlingMovementParameters.InitialLocation = GetActorLocation();
	MantlingMovementParameters.InitialRotation = BaseCharacterMovementComponent->IsClimbingLadder()
		                                             ? FRotator(0, LedgeDescription.Rotation.Yaw, 0)
		                                             : GetActorRotation();
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
	FOLedgeDescription LedgeDescription;
	if (LedgeDetectorComponent->TryDetectLedge(LedgeDescription))
	{
		FOMantlingMovementParameters MantlingMovementParameters;
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

bool AOBaseCharacter::IsWeaponInHand() const
{
	return WeaponComponent->GetWeaponType() != EOEquippableItemType::None;
}

void AOBaseCharacter::BindOnChangePrimaryAttribute(EOPrimaryAttr Type, UObject* Object, FName Name, bool bWithUpdate)
{
	if (!IsValid(Object))
	{
		return;
	}
	switch (Type)
	{
		case EOPrimaryAttr::Health:
		{
			PrimaryAttributesComponent->OnChangeHealth.AddUFunction(Object, Name);
			if (bWithUpdate)
			{
				PrimaryAttributesComponent->UpdateHealth();
			}
			break;
		}
		case EOPrimaryAttr::Stamina:
		{
			PrimaryAttributesComponent->OnChangeStamina.AddUFunction(Object, Name);
			if (bWithUpdate)
			{
				PrimaryAttributesComponent->UpdateStamina();
			}
			break;
		}
		case EOPrimaryAttr::Die:
		{
			PrimaryAttributesComponent->OnDie.AddUFunction(Object, Name);
			break;
		}
		default: ;
	}
}

FOnNotifyChangeWeapon& AOBaseCharacter::GetOnNotifyChangeWeapon()
{
	return WeaponComponent->OnNotifyChangeWeapon;
}

FOnNotifyUpdatedAmmoWeapon& AOBaseCharacter::GetOnNotifyUpdatedAmmoWeapon()
{
	return WeaponComponent->OnNotifyUpdatedAmmoWeapon;
}

bool AOBaseCharacter::TryAddAmmo(const EOAmmoType& Type, int32 Count)
{
	return WeaponComponent->AddAmmo(Type, Count);
}

bool AOBaseCharacter::TryAddBoosters(const EOBoostingType& Type, int32 Value)
{
	bool bResult = false;
	switch (Type)
	{
		case EOBoostingType::Health:
		{
			bResult = PrimaryAttributesComponent->TryAddHealth(Value);
			break;
		}
		case EOBoostingType::Stamina:
		{
			bResult = PrimaryAttributesComponent->TryAddStamina(Value);
		}
		default: ;
	}
	return bResult;
}

bool AOBaseCharacter::IsDie() const
{
	return PrimaryAttributesComponent->IsDead();
}

void AOBaseCharacter::OnDie()
{
	StopAnimMontage();
	PlayAnimMontage(DeathAnimMontage);
	GetCharacterMovement()->DisableMovement();
	SetLifeSpan(5.f);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponComponent->StopFire();
	if (Controller)
	{
		Controller->DisableInput(nullptr);
		Controller->ChangeState(NAME_Spectating);
	}
}

void AOBaseCharacter::OnChangeHealth(float Health, float Diff, float MaxValue)
{
	if (!PrimaryAttributesComponent->IsDead() && Diff < 0)
	{
		const FOWeaponAnimDescription Desc = WeaponComponent->GetWeaponAnimDescription();
		PlayAnimMontage(Desc.HitAnimMontage);
	}
}

void AOBaseCharacter::SetCharacterColor(const FLinearColor& LinearColor)
{
	const auto MaterialInst = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
	if (!MaterialInst)
	{
		return;
	}
	MaterialInst->SetVectorParameterValue(MaterialColorName, LinearColor);
}

