// Fill out your copyright notice in the Description page of Project Settings.


#include "OPlayerController.h"

#include "Origin/Characters/OBaseCharacter.h"
#include "Origin/Characters/OPlayerCharacter.h"

void AOPlayerController::SetPawn(APawn* InPawn)
{
    Super::SetPawn(InPawn);
    CachedBaseCharacter = Cast<AOBaseCharacter>(InPawn);
    if (CachedBaseCharacter.IsValid() && CachedBaseCharacter->IsA<AOPlayerCharacter>())
    {
        const AOPlayerCharacter* PlayerCharacter = Cast<AOPlayerCharacter>(CachedBaseCharacter.Get());
        if (PlayerCameraManager)
        {
            PlayerCameraManager->ViewPitchMin = PlayerCharacter->GetViewPitchMin();
            PlayerCameraManager->ViewPitchMax = PlayerCharacter->GetViewPitchMax();
        }      
    }
}

void AOPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindAxis("MoveForward", this, &AOPlayerController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AOPlayerController::MoveRight);
    InputComponent->BindAxis("Turn", this, &AOPlayerController::Turn);
    InputComponent->BindAxis("LookUp", this, &AOPlayerController::LookUp);
    InputComponent->BindAxis("TurnAtRate", this, &AOPlayerController::TurnAtRate);
    InputComponent->BindAxis("LookUpAtRate", this, &AOPlayerController::LookUpAtRate);
    InputComponent->BindAxis("SwimRight", this, &AOPlayerController::MoveSwimRight);
    InputComponent->BindAxis("SwimUp", this, &AOPlayerController::MoveSwimUp);
    InputComponent->BindAxis("SwimForward", this, &AOPlayerController::MoveSwimForward);
    InputComponent->BindAxis("ClimbLadder", this, &AOPlayerController::ClimbLadder);
    InputComponent->BindAction("InteractionWithLadder", EInputEvent::IE_Pressed, this, &AOPlayerController::InteractionWithLadder);
    InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &AOPlayerController::Mantle);
    InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AOPlayerController::Jump);
    InputComponent->BindAction("Crawl", EInputEvent::IE_Pressed, this, &AOPlayerController::ChangeCrawlState);
    InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AOPlayerController::ChangeCrouchState);
    InputComponent->BindAction<FChangeSprint>("Sprint", EInputEvent::IE_Pressed, this, &AOPlayerController::ChangeSprint, true);
    InputComponent->BindAction<FChangeSprint>("Sprint", EInputEvent::IE_Released, this, &AOPlayerController::ChangeSprint, false);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AOPlayerController::Fire);
}

void AOPlayerController::ChangeSprint(bool bWantsToSprint)
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->ChangeSprint(bWantsToSprint);
    }
}

void AOPlayerController::LookUp(float Value)
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->LookUp(Value);
    }
}

void AOPlayerController::Turn(float Value)
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->Turn(Value);
    }
}

void AOPlayerController::MoveRight(float Value)
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->MoveRight(Value);
    }
}

void AOPlayerController::MoveForward(float Value)
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->MoveForward(Value);
    }
}

void AOPlayerController::LookUpAtRate(float Value)
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->LookUpAtRate(Value);
    }
}

void AOPlayerController::TurnAtRate(float Value)
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->TurnAtRate(Value);
    }
}

void AOPlayerController::Jump()
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->Jump();
    }
}

void AOPlayerController::ChangeCrouchState()
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->ChangeCrouchState();
    }
}

void AOPlayerController::ChangeCrawlState()
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->ChangeCrawlState();
    }
}

void AOPlayerController::MoveSwimRight(float Value)
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->MoveSwimRight(Value);
    }
}

void AOPlayerController::MoveSwimForward(float Value)
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->MoveSwimForward(Value, LastSwimUpValue);
    }
}

void AOPlayerController::MoveSwimUp(float Value)
{
    if (CachedBaseCharacter.IsValid())
    {
        LastSwimUpValue = Value;
        CachedBaseCharacter->MoveSwimUp(Value);
    }
}

void AOPlayerController::Mantle()
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->Mantle();
    }
}

void AOPlayerController::ClimbLadder(float Value)
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->ClimbLadder(Value);
    }
}

void AOPlayerController::InteractionWithLadder()
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->InteractionWithLadder();
    }
}

void AOPlayerController::Fire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Fire();
	}
}
