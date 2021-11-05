// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePawnAnimInstance.h"

#include "Origin/Characters/GameCodeBasePawn.h"
#include "GameFramework/PawnMovementComponent.h"

void UBasePawnAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    checkf(TryGetPawnOwner()->IsA<AGameCodeBasePawn>(), TEXT("UBasePawnAnimInstance::NativeBeginPlay() work with only AGameCodeBasePawn and him child"));
    CurrentPawn = StaticCast<AGameCodeBasePawn*>(TryGetPawnOwner());
}

void UBasePawnAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (CurrentPawn.IsValid())
    {
        InputForward = CurrentPawn->GetInputForward();
        InputRight = CurrentPawn->GetInputRight();
        bIsOnAir = CurrentPawn->MovementComponent->IsFalling();
    }
}
