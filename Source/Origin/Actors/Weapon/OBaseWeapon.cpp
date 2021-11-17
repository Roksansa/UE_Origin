// Fill out your copyright notice in the Description page of Project Settings.


#include "OBaseWeapon.h"

#include "DrawDebugHelpers.h"
#include "OTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

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
}


EEquippableItemType AOBaseWeapon::GetItemType() const
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
	return ViewRotationVector;
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
	
	const FVector MuzzleLocation = GetMuzzleWorldLocation();
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Firing, CollisionQueryParams);
	if (HitResult.bBlockingHit)
	{
		DrawDebugLine(GetWorld(), MuzzleLocation, HitResult.ImpactPoint, FColor::Green, false, 3.f, 0, 3.f);
	}

	if (bComplexTrace)
	{
		FHitResult HitResultNew;
		//create new line through hit impact point previous step (HitResult)
		const FVector NewTraceEnd = HitResult.bBlockingHit ? TraceStart + (HitResult.ImpactPoint - MuzzleLocation).GetSafeNormal()*TraceMaxDistance : TraceEnd;
		GetWorld()->LineTraceSingleByChannel(HitResultNew, MuzzleLocation, NewTraceEnd, ECC_Firing, CollisionQueryParams);
		HitResult = HitResultNew.bBlockingHit ? HitResultNew : HitResult;   
	}
	
	DrawDebugSphere(GetWorld(), HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd, bComplexTrace ? 15.f : 10.f, 24, bComplexTrace ? FColor::Cyan : FColor::Magenta, false, 5.f);
	DrawDebugLine(GetWorld(), MuzzleLocation, HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd , FColor::Orange, false, 3.f, 0, 3.f);
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

void AOBaseWeapon::MakeDamage(const FHitResult& HitResult) const
{
	AActor* Actor = HitResult.GetActor();
	if (IsValid(Actor))
	{
		Actor->TakeDamage(DamageAmount, FPointDamageEvent{}, GetInstigatorController(), GetOwner());
	}
}
