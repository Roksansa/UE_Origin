// Fill out your copyright notice in the Description page of Project Settings.


#include "OLauncherWeapon.h"

#include "OProjectile.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void AOLauncherWeapon::StartFire()
{
	MakeShot();
}

void AOLauncherWeapon::StopFire()
{
	Super::StopFire();
}

void AOLauncherWeapon::MakeShot()
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
	

	const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
	const FVector Direction = (EndPoint - GetMuzzleWorldLocation()).GetSafeNormal();

	
	const FTransform SpawnTransform(FRotator::ZeroRotator, GetMuzzleWorldLocation());
	ACharacter* Player = Cast<ACharacter>(GetOwner());
	AOProjectile* Projectile = GetWorld()->SpawnActorDeferred<AOProjectile>(ProjectileType, SpawnTransform, GetOwner(), Player);
	if (Projectile)
	{
		Projectile->SetDamageAmount(DamageAmount);
		Projectile->SetShotDirection(Direction);
		Projectile->FinishSpawning(SpawnTransform);
	}
	OnMakeShot.Broadcast();
}
