// Fill out your copyright notice in the Description page of Project Settings.


#include "OTraceUtils.h"

#include "DrawDebugHelpers.h"

bool OTraceUtils::SweepCapsuleSingleByChannel(const UWorld* InWorld, struct FHitResult& OutHit, const FVector& Start, const FVector& End, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params, const FCollisionResponseParams& ResponseParam, bool bDrawDebug, float DrawTime, FColor TraceColor, FColor HitColor)
{
	bool bResult = false;

	const FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	bResult = InWorld->SweepSingleByChannel(OutHit, Start, End, Rot, TraceChannel, CapsuleShape, Params, ResponseParam);

#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		DrawDebugCapsule(InWorld, Start, CapsuleHalfHeight, CapsuleRadius, Rot, TraceColor, false, DrawTime);
		DrawDebugCapsule(InWorld, End, CapsuleHalfHeight, CapsuleRadius, Rot, TraceColor, false, DrawTime);
		if (bResult)
		{
			DrawDebugCapsule(InWorld, OutHit.Location, CapsuleHalfHeight, CapsuleRadius, Rot, HitColor, false, DrawTime);
			DrawDebugPoint(InWorld, OutHit.ImpactPoint, 20.f, HitColor, false, DrawTime);
		}
	}
#endif
	
	return bResult;
}

bool OTraceUtils::SweepBoxSingleByChannel(const UWorld* InWorld, struct FHitResult& OutHit, const FVector& Start, const FVector& End, FVector& BoxHalfExtent, const FQuat& Rot, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params, const FCollisionResponseParams& ResponseParam, bool bDrawDebug, float DrawTime, FColor TraceColor, FColor HitColor)
{
	bool bResult = false;

	const FCollisionShape BoxShape = FCollisionShape::MakeBox(BoxHalfExtent);
	bResult = InWorld->SweepSingleByChannel(OutHit, Start, End, Rot, TraceChannel, BoxShape, Params, ResponseParam);

#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		DrawDebugBox(InWorld, Start, BoxHalfExtent, Rot, TraceColor, false, DrawTime);
		DrawDebugBox(InWorld, End, BoxHalfExtent, Rot, TraceColor, false, DrawTime);
		if (bResult)
		{
			DrawDebugBox(InWorld, OutHit.Location, BoxHalfExtent, Rot, HitColor, false, DrawTime);
			DrawDebugPoint(InWorld, OutHit.ImpactPoint, 10.f, HitColor, false, DrawTime);
		}
	}
#endif
	
	return bResult;
}


bool OTraceUtils::SweepSphereSingleByChannel(const UWorld* InWorld, FHitResult& OutHit, const FVector& Start, const FVector& End, float SphereRadius, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params, const FCollisionResponseParams& ResponseParam, bool bDrawDebug, float DrawTime, FColor TraceColor, FColor HitColor)
{
	bool bResult = false;

	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);
	bResult = InWorld->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, TraceChannel, SphereShape, Params, ResponseParam);

#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		const FVector StartDrawCapsuleLocation = (Start + End) * 0.5f;
		const FVector TraceVector = End - Start;
		const float StartDrawCapsuleHalfHeight = (TraceVector).Size() * 0.5f;
		const FQuat DebugCapsuleRotation = FRotationMatrix::MakeFromZ(TraceVector).ToQuat();
		DrawDebugCapsule(InWorld, StartDrawCapsuleLocation, StartDrawCapsuleHalfHeight, SphereRadius, DebugCapsuleRotation, TraceColor, false, DrawTime);
		if (bResult)
		{
			DrawDebugSphere(InWorld, OutHit.Location, SphereRadius, 32, HitColor, false, DrawTime);
			DrawDebugPoint(InWorld, OutHit.ImpactPoint, 20.f, HitColor, false, DrawTime);
		}
	}
#endif
	
	return bResult;
}

bool OTraceUtils::OverlapCapsuleAnyByProfile(const UWorld* InWorld, const FVector& Pos, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot, FName ProfileName, const FCollisionQueryParams& Params, bool bDrawDebug, float DrawTime, FColor TraceColor, FColor HitColor)
{
	bool bResult = false;
	
	const FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);	
	bResult = InWorld->OverlapAnyTestByProfile(Pos, Rot, ProfileName, CapsuleShape, Params);
	
#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		DrawDebugCapsule(InWorld, Pos, CapsuleHalfHeight, CapsuleRadius, Rot, !bResult ? TraceColor : HitColor, false, DrawTime);
	}
#endif
	
	return bResult;
}

bool OTraceUtils::OverlapCapsuleBlockingByProfile(const UWorld* InWorld, const FVector& Pos, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot, FName ProfileName, const FCollisionQueryParams& Params, bool bDrawDebug, float DrawTime, FColor TraceColor, FColor HitColor)
{
	bool bResult = false;
	
	const FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);	
	bResult = InWorld->OverlapBlockingTestByProfile(Pos, Rot, ProfileName, CapsuleShape, Params);
	
#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		DrawDebugCapsule(InWorld, Pos, CapsuleHalfHeight, CapsuleRadius+5.f, Rot, !bResult ? TraceColor : HitColor, false, DrawTime, 0, 3.f);
	}
#endif
	
	return bResult;
}