// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * Draw Utils 
 */
namespace OTraceUtils
{
	bool SweepCapsuleSingleByChannel(const UWorld* InWorld, struct FHitResult& OutHit, const FVector& Start, const FVector& End, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam, bool bDrawDebug = false, float DrawTime = -1.f, FColor TraceColor = FColor::Black, FColor HitColor = FColor::Red);
	bool SweepSphereSingleByChannel(const UWorld* InWorld, struct FHitResult& OutHit, const FVector& Start, const FVector& End, float SphereRadius, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam, bool bDrawDebug = false, float DrawTime = -1.f, FColor TraceColor = FColor::Black, FColor HitColor = FColor::Red);

	bool OverlapCapsuleAnyByProfile(const UWorld* InWorld, const FVector& Pos, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot, FName ProfileName, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, bool bDrawDebug = false, float DrawTime = -1.f, FColor TraceColor = FColor::Green, FColor HitColor = FColor::Cyan);
	bool OverlapCapsuleBlockingByProfile(const UWorld* InWorld, const FVector& Pos, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot, FName ProfileName, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, bool bDrawDebug = false, float DrawTime = -1.f, FColor TraceColor = FColor::Green, FColor HitColor = FColor::Cyan);
}