// Fill out your copyright notice in the Description page of Project Settings.


#include "ORifleWeapon.h"

DEFINE_LOG_CATEGORY_STATIC(LogRifleWeapon, All, All);

void AORifleWeapon::StartFire()
{
	UE_LOG(LogRifleWeapon, Display, TEXT("Fire"));
	if (GetWorldTimerManager().GetTimerElapsed(ShotTimerHandle) > 0.f)
	{
		const float TimerLeft = GetWorldTimerManager().GetTimerRate(ShotTimerHandle) - GetWorldTimerManager().GetTimerElapsed(ShotTimerHandle);
		GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &AORifleWeapon::MakeShot, TimerLeft, false);
	}
	else
	{
		MakeShot();
	}
}

void AORifleWeapon::StopFire()
{
	Super::StopFire();
	GetWorldTimerManager().ClearTimer(ShotTimerHandle);
}

void AORifleWeapon::MakeShot()
{
	if (!GetWorld())
	{
		return;
	}
	if (IsAmmoEmpty())
	{
		if (bIsAutoReload)
		{
			OnWasOutOfBullets.Broadcast();
		}
		StopFire();
		return;
	}
	
	FVector TraceStartViewPoint;
	FVector TraceEnd;

	if (!GetTraceData(TraceStartViewPoint, TraceEnd))
	{
		return;
	}

	FHitResult HitResult;
	MakeHit(HitResult, TraceStartViewPoint, TraceEnd);
	
	if (HitResult.bBlockingHit)
	{
		MakeDamage(HitResult);
	}
	OnMakeShot.Broadcast();
	DecreaseAmmo();
	GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &AORifleWeapon::MakeShot, TimerBetweenShots, false);
}

FVector AORifleWeapon::GetShootDirection(const FVector& ViewRotationVector) const
{
	const float HalfRad = FMath::DegreesToRadians(BulletSpread);
	return FMath::VRandCone(ViewRotationVector,HalfRad);
}
