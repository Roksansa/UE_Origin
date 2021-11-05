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
}

void AOBaseWeapon::Fire()
{
	UE_LOG(LogBaseWeapon, Display, TEXT("Fire"));
	MakeShot();
}

void AOBaseWeapon::MakeShot()
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
	const FVector ShootDirection = ViewRotationVector;
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
