// Fill out your copyright notice in the Description page of Project Settings.


#include "ORifleWeapon.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Utils/OSpawnUtils.h"

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
	if (GetWorldTimerManager().GetTimerElapsed(ShotTimerHandle) > 0.f)
	{
		const float TimerLeft = GetWorldTimerManager().GetTimerRate(ShotTimerHandle) - GetWorldTimerManager().GetTimerElapsed(ShotTimerHandle);
		GetWorldTimerManager().SetTimer(ShotTimerHandle,
			[this](){
			GetWorldTimerManager().ClearTimer(ShotTimerHandle);
			}
			, TimerLeft, false);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(ShotTimerHandle);
	}
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
		SpawnVFX(GetWorld(),HitResult, DefaultImpactData, ImpactsByPhys);
	}
	SpawnTraceEffect(GetMuzzleWorldLocation(), HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd);
	OnMakeShot.Broadcast();
	DecreaseAmmo();
	GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &AORifleWeapon::MakeShot, TimerBetweenShots, false);
}

void AORifleWeapon::SpawnTraceEffect(const FVector& TraceStart, const FVector& TraceEnd)
{
	const auto TraceComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceEffect, TraceStart);
	if (TraceComponent)
	{
		TraceComponent->SetNiagaraVariableVec3(TraceTargetName, TraceEnd);
	}
}
