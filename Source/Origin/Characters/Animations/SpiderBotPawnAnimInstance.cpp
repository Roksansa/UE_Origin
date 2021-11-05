// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderBotPawnAnimInstance.h"

#include "Origin/Characters/SpiderBotPawn.h"

void USpiderBotPawnAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    checkf(TryGetPawnOwner()->IsA<ASpiderBotPawn>(), TEXT("USpiderBotPawnAnimInstance::NativeBeginPlay() work with only ASpiderBotPawn and him child"));
    CurrentPawn = StaticCast<ASpiderBotPawn*>(TryGetPawnOwner());
}

void USpiderBotPawnAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (CurrentPawn.IsValid())
    {
        RightFrontFootEffectorLocation = FVector(CurrentPawn->GetIKRightFrontFootOffset(),0,0);
        RightRearFootEffectorLocation = FVector(CurrentPawn->GetIKRightRearFootOffset(),0,0);
        LeftFrontFootEffectorLocation = FVector(CurrentPawn->GetIKLeftFrontFootOffset(),0,0);
        LeftRearFootEffectorLocation = FVector(CurrentPawn->GetIKLeftRearFootOffset(),0,0);
    }
}
