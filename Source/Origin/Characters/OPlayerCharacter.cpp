// Fill out your copyright notice in the Description page of Project Settings.


#include "OPlayerCharacter.h"


#include "Camera/CameraComponent.h"
#include "Components/OPrimaryAttributesComponent.h"
#include "Components/OWeaponComponent.h"
#include "Controllers/OPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Origin/Actors/OLadderInteractiveActor.h"
#include "Origin/MovementComponents/OBaseCharacterMovementComponent.h"

AOPlayerCharacter::AOPlayerCharacter()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SpringArmComponent->bUsePawnControlRotation = true;
	CameraComponent->bUsePawnControlRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);
	GetCharacterMovement()->CrouchedHalfHeight = 65.f;
}

void AOPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	AOPlayerController* CurrentPlayerController = Cast<AOPlayerController>(GetController());
	if (CurrentPlayerController != nullptr)
	{
		CurrentPlayerController->BindWidgets();
	}
	CurrentFOV = CameraComponent->FieldOfView;
}

void AOPlayerCharacter::LookUp(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1E-06f) && !BaseCharacterMovementComponent->IsClimbingLadder())
	{
		AddControllerPitchInput(Value);
	}
}

void AOPlayerCharacter::Turn(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1E-06f) && !BaseCharacterMovementComponent->IsClimbingLadder())
	{
		AddControllerYawInput(Value);
	}
}

void AOPlayerCharacter::MoveRight(float Value)
{
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1E-06f))
	{
		const FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		const FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void AOPlayerCharacter::MoveForward(float Value)
{
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1E-06f))
	{
		const FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		const FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void AOPlayerCharacter::TurnAtRate(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1E-06f) && !BaseCharacterMovementComponent->IsClimbingLadder())
	{
		AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AOPlayerCharacter::LookUpAtRate(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1E-06f))
	{
		AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void AOPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	float MeshRelativeLocationZ = 0.f;
	if (GetMesh())
	{
		MeshRelativeLocationZ = GetMesh()->GetRelativeLocation().Z;
	}
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	if (GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = MeshRelativeLocationZ + HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}

	SpringArmComponent->TargetOffset += FVector(0.f, 0.f, HalfHeightAdjust);
}

void AOPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	float MeshRelativeLocationZ = 0.f;
	if (GetMesh())
	{
		MeshRelativeLocationZ = GetMesh()->GetRelativeLocation().Z;
	}
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	if (GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = MeshRelativeLocationZ - HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	SpringArmComponent->TargetOffset -= FVector(0.f, 0.f, HalfHeightAdjust);
}

void AOPlayerCharacter::OnStartCrawling(float HalfHeightAdjust, float ScaledHeightAdjust)
{
	Super::OnStartCrawling(HalfHeightAdjust, ScaledHeightAdjust);
	SpringArmComponent->TargetOffset += FVector(0.f, 0.f, HalfHeightAdjust);
	if (GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = MeshRelativeLocation.Z + HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
		MeshRelativeLocation.X = MeshRelativeLocation.X - LocationXMeshOffsetForCrawling;
		BaseTranslationOffset.X = MeshRelativeLocation.X;
	}
}

void AOPlayerCharacter::OnEndCrawling(float HalfHeightAdjust, float ScaledHeightAdjust)
{
	Super::OnEndCrawling(HalfHeightAdjust, ScaledHeightAdjust);
	SpringArmComponent->TargetOffset -= FVector(0.f, 0.f, HalfHeightAdjust);
	if (GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = MeshRelativeLocation.Z - HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
		MeshRelativeLocation.X = MeshRelativeLocation.X + LocationXMeshOffsetForCrawling;
		BaseTranslationOffset.X = MeshRelativeLocation.X;
	}
}

bool AOPlayerCharacter::CanJumpInternal_Implementation() const
{
	//test debug message for correct use jump and within variable - now ok
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, FString::Format(TEXT("{0} - {1} - {2}"), {bIsCrouched, BaseCharacterMovementComponent->CanUnCrouch(), Super::CanJumpInternal_Implementation()}));
	return !PrimaryAttributesComponent->GetIsOutOfStamina() && (bIsCrouched && BaseCharacterMovementComponent->CanGrowCapsule() ||
	                            Super::CanJumpInternal_Implementation());
}

void AOPlayerCharacter::OnJumped_Implementation()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void AOPlayerCharacter::MoveSwimRight(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1E-06f))
	{
		const FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		const FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void AOPlayerCharacter::MoveSwimForward(float Value, float LastSwimUpValue)
{
	if (GetCharacterMovement()->IsSwimming() && FMath::IsNearlyZero(LastSwimUpValue, 1E-06f) && !FMath::IsNearlyZero(Value, 1E-06f))
	{
		const FRotator PitchYawRotator(!bIsOverlapVolumeSurface ? GetControlRotation().Pitch : 0.f, GetControlRotation().Yaw, 0.f);
		const FVector ForwardVector = PitchYawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void AOPlayerCharacter::MoveSwimUp(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1E-06f))
	{
		const FRotator YawRotator(0.f, GetControlRotation().Yaw, 0);
		const FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		const float Sign = FMath::Sign(Value);
		const FVector UpVector = FVector(ForwardVector.X * Sign, ForwardVector.Y * Sign, 5.0f);
		if (bIsOverlapVolumeSurface)
		{
			GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Blue,
				FString::Format(TEXT(" bIsOverlapVolumeSurface {0} "), {bIsOverlapVolumeSurface}), true, FVector2D(2, 2));
			FVector JumpDir(0.f);
			FVector WallNormal(0.f);
			if (BaseCharacterMovementComponent->ShouldJumpOutOfWater(JumpDir) &&
			    BaseCharacterMovementComponent->CheckWaterJump(JumpDir, WallNormal))
			{
				GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red,
					FString::Format(TEXT(" bIsOverlapVolumeSurface {0} {1}"), {bIsOverlapVolumeSurface, Value}), true, FVector2D(2, 2));
				AddMovementInput(UpVector, Value);
				return;
			}
		}

		AddMovementInput(UpVector, !bIsOverlapVolumeSurface || Value < -0.01f ? Value : 0.f);
	}
}

void AOPlayerCharacter::OnStartSwimming(float HalfHeightAdjust, float ScaledHeightAdjust)
{
	Super::OnStartSwimming(HalfHeightAdjust, ScaledHeightAdjust);

	bUseControllerRotationYaw = false;
	BaseCharacterMovementComponent->bOrientRotationToMovement = true;

	const AOPlayerCharacter* DefaultChar = GetDefault<AOPlayerCharacter>(GetClass());
	SpringArmComponent->TargetOffset = DefaultChar->SpringArmComponent->TargetOffset;
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
		MeshRelativeLocation.X = DefaultChar->GetMesh()->GetRelativeLocation().X;
		BaseTranslationOffset.X = MeshRelativeLocation.X;
	}
}

void AOPlayerCharacter::OnEndSwimming(float HalfHeightAdjust, float ScaledHeightAdjust)
{
	Super::OnEndSwimming(HalfHeightAdjust, ScaledHeightAdjust);

	bUseControllerRotationYaw = true;
	BaseCharacterMovementComponent->bOrientRotationToMovement = false;

	const AOPlayerCharacter* DefaultChar = GetDefault<AOPlayerCharacter>(GetClass());
	SpringArmComponent->TargetOffset = DefaultChar->SpringArmComponent->TargetOffset;
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
		MeshRelativeLocation.X = DefaultChar->GetMesh()->GetRelativeLocation().X;
		BaseTranslationOffset.X = MeshRelativeLocation.X;
	}
}

void AOPlayerCharacter::ClimbLadder(float Value)
{
	if (BaseCharacterMovementComponent->IsClimbingLadder() && !FMath::IsNearlyZero(Value))
	{
		const FVector ClimbLadderVector = BaseCharacterMovementComponent->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(ClimbLadderVector, Value);
	}
}

void AOPlayerCharacter::StartFire()
{
	Super::StartFire();
	if (WeaponComponent->GetWeaponType() == EOEquippableItemType::None || !CanUseWeapon())
	{
		return;
	}
	bWantFire = true;
	GetWorld()->GetTimerManager().ClearTimer(CheckFireTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(CheckFireTimerHandle, [this]()
	{
		const bool bAllowFire = CanUseWeapon();

		if (bAllowFire && bWantFire && WeaponComponent->GetState() == EOWeaponUseState::Idle)
		{
			OnAllowFire.Broadcast(bAllowFire);
			return;
		}

		if (!bAllowFire && WeaponComponent->GetState() == EOWeaponUseState::Fire)
		{
			OnAllowFire.Broadcast(bAllowFire);
		}
	}, 0.5f, true);
}

void AOPlayerCharacter::StopFire()
{
	Super::StopFire();
	GetWorld()->GetTimerManager().ClearTimer(CheckFireTimerHandle);
	bWantFire = false;
}

float AOPlayerCharacter::GetViewPitchMin() const
{
	return ViewPitchMin;
}

float AOPlayerCharacter::GetViewPitchMax() const
{
	return ViewPitchMax;
}

void AOPlayerCharacter::OnDie()
{
	Super::OnDie();
	GetWorld()->GetTimerManager().ClearTimer(CheckFireTimerHandle);
}
