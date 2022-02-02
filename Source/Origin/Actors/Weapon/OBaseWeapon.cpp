// Fill out your copyright notice in the Description page of Project Settings.


#include "OBaseWeapon.h"

#include "DrawDebugHelpers.h"
#include "OTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

AOBaseWeapon::AOBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
}

void AOBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	check(WeaponMesh);
	OnMakeShot.AddUObject(this, &AOBaseWeapon::PlayVisibleShot);
	CurrentBulletSpread = BulletSpread;
}


void AOBaseWeapon::StopFire()
{
	OnStopFire.Broadcast();
}

void AOBaseWeapon::StartAim()
{
	CurrentBulletSpread = AimBulletSpread;
}

void AOBaseWeapon::StopAim()
{
	CurrentBulletSpread = BulletSpread;
}

EOEquippableItemType AOBaseWeapon::GetItemType() const
{
	return EquippableItemType;
}

APlayerController* AOBaseWeapon::GetPlayerController() const
{
	const ACharacter* Player = Cast<ACharacter>(GetOwner());
	if (!Player)
	{
		return nullptr;
	}

	return Player->GetController<APlayerController>();
}

bool AOBaseWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const
{
	const APlayerController* Controller = GetPlayerController();
	if (!Controller)
	{
		return false;
	}

	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	return true;
}

FVector AOBaseWeapon::GetMuzzleWorldLocation() const
{
	return WeaponMesh->GetSocketTransform(MuzzleSocketName).GetLocation();
}

FVector AOBaseWeapon::GetShootDirection(const FVector& ViewRotationVector) const
{
	const float HalfRad = FMath::DegreesToRadians(CurrentBulletSpread);
	return FMath::VRandCone(ViewRotationVector,HalfRad);
}

bool AOBaseWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
	FVector ViewLocation;
	FRotator ViewRotation;
	const bool bInit = GetPlayerViewPoint(ViewLocation, ViewRotation);
	if (!bInit)
	{
		return false;
	}

	const FVector ViewRotationVector = ViewRotation.Vector();
	const FVector MuzzleLocation = GetMuzzleWorldLocation();
	TraceStart = ViewLocation + ViewRotationVector * ((MuzzleLocation - ViewLocation) | ViewRotationVector);
	const FVector ShootDirection = GetShootDirection(ViewRotationVector);
	TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;
	return true;
}

void AOBaseWeapon::MakeHit(FHitResult& HitResult, const FVector TraceStart, const FVector TraceEnd) const
{
	if (!GetWorld())
	{
		return;
	}

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetOwner());
	CollisionQueryParams.bReturnPhysicalMaterial = true;
	
	const FVector MuzzleLocation = GetMuzzleWorldLocation();
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Firing, CollisionQueryParams);

	if (bComplexTrace)
	{
		FHitResult HitResultNew;
		//create new line through hit impact point previous step (HitResult)
		const FVector NewTraceEnd = HitResult.bBlockingHit ? TraceStart + (HitResult.ImpactPoint - MuzzleLocation).GetSafeNormal()*TraceMaxDistance : TraceEnd;
		GetWorld()->LineTraceSingleByChannel(HitResultNew, MuzzleLocation, NewTraceEnd, ECC_Firing, CollisionQueryParams);
		HitResult = HitResultNew.bBlockingHit ? HitResultNew : HitResult;   
	}
}

void AOBaseWeapon::PlayVisibleShot()
{
	PlayAnimMontage(FireAnimMontage);
}

float AOBaseWeapon::PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	UAnimInstance * AnimInstance = (WeaponMesh)? WeaponMesh->GetAnimInstance() : nullptr; 
	if( AnimMontage && AnimInstance )
	{
		float const Duration = AnimInstance->Montage_Play(AnimMontage, InPlayRate);

		if (Duration > 0.f)
		{
			// Start at a given Section.
			if( StartSectionName != NAME_None )
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
			}

			return Duration;
		}
	}	

	return 0.f;
}

void AOBaseWeapon::DecreaseAmmo()
{
	CurrentBullets--;
	OnChangeBullets.Broadcast();
}

bool AOBaseWeapon::IsAmmoEmpty() const
{
	return CurrentBullets == 0;
}

const FOAmmoData& AOBaseWeapon::GetAmmoData() const
{
	return DefaultAmmo;
}

int32 AOBaseWeapon::GetCountBullets() const
{
	return CurrentBullets;
}

bool AOBaseWeapon::SetCountBullets(int32 NewCount)
{
	const int32 TempCount = CurrentBullets;
	CurrentBullets = FMath::Clamp(CurrentBullets + NewCount, 0, DefaultAmmo.BulletsInClip);
	const bool bChange = TempCount == CurrentBullets - NewCount || TempCount != CurrentBullets;
	if (bChange)
	{
		OnChangeBullets.Broadcast();
	}
	return bChange;
}

bool AOBaseWeapon::IsAutoReload() const
{
	return bIsAutoReload;
}

bool AOBaseWeapon::IsFullAuto() const
{
	return bIsFullAuto;
}

float AOBaseWeapon::GetAimFOV() const
{
	return AimFOV;
}

void AOBaseWeapon::MakeDamage(const FHitResult& HitResult) const
{
	AActor* Actor = HitResult.GetActor();
	if (IsValid(Actor))
	{
		Actor->TakeDamage(DamageAmount, FPointDamageEvent{}, GetInstigatorController(), GetOwner());
	}
}
