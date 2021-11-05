// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderBotPawn.h"

#include "Kismet/KismetSystemLibrary.h"


ASpiderBotPawn::ASpiderBotPawn()
{
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SpiderMesh"));
    SkeletalMeshComponent->SetupAttachment(RootComponent);

    IKScale = GetActorScale3D().Z;
    IKTraceDistance = CollisionSphereRadius * IKScale;
}

void ASpiderBotPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    IKRightFrontFootOffset = FMath::FInterpTo(IKRightFrontFootOffset, GetIKOffsetForSocketName(RightFrontFootSocketName), DeltaSeconds, IKInterpSpeed);
    IKRightRearFootOffset = FMath::FInterpTo(IKRightRearFootOffset, GetIKOffsetForSocketName(RightRearFootSocketName), DeltaSeconds, IKInterpSpeed);
    IKLeftFrontFootOffset = FMath::FInterpTo(IKLeftFrontFootOffset, GetIKOffsetForSocketName(LeftFrontFootSocketName), DeltaSeconds, IKInterpSpeed);
    IKLeftRearFootOffset = FMath::FInterpTo(IKLeftRearFootOffset, GetIKOffsetForSocketName(LeftRearFootSocketName), DeltaSeconds, IKInterpSpeed);
}

float ASpiderBotPawn::GetIKOffsetForSocketName(const FName& SocketName) const
{
    float Result = 0.f;
    
    const FVector SocketLocation = SkeletalMeshComponent->GetSocketLocation(SocketName);
    const FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
    const FVector TraceEnd = TraceStart - IKTraceDistance * FVector::UpVector;

    FHitResult HitResult;
    const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
    if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
    {
        Result = (TraceEnd.Z - HitResult.Location.Z) / IKScale;
    }
    else if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceEnd, TraceEnd - IKExtendDistance * FVector::UpVector, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
    {
        Result = (TraceEnd.Z - HitResult.Location.Z) / IKScale;
    }
    return Result;
}
