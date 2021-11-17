// Fill out your copyright notice in the Description page of Project Settings.


#include "ORifleWeapon.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

void AORifleWeapon::StartFire()
{
	UE_LOG(LogBaseWeapon, Display, TEXT("Fire"));
	GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &AORifleWeapon::MakeShot, TimerBetweenShots, true);
	MakeShot();
}

void AORifleWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(ShotTimerHandle);
}

void AORifleWeapon::MakeShot()
{
	if (!GetWorld())
	{
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
}

FVector AORifleWeapon::GetShootDirection(const FVector& ViewRotationVector) const
{
	const float HalfRad = FMath::DegreesToRadians(BulletSpread);
	return FMath::VRandCone(ViewRotationVector,HalfRad);
}
