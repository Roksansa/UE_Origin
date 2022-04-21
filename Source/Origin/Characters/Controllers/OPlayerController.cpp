// Fill out your copyright notice in the Description page of Project Settings.

#include "OPlayerController.h"

#include "OTypes.h"
#include "Components/OPlayerRespawnComponent.h"
#include "Components/OWidgetManagerComponent.h"
#include "GameFramework/PlayerInput.h"
#include "Origin/Characters/OBaseCharacter.h"
#include "Origin/Characters/OPlayerCharacter.h"

AOPlayerController::AOPlayerController(const FObjectInitializer& ObjectInitializer)
{
	WidgetManager = CreateDefaultSubobject<UOWidgetManagerComponent>("WidgetManager");
	RespawnComponent = CreateDefaultSubobject<UOPlayerRespawnComponent>("RespawnComponent");
}

void AOPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn && CachedBaseCharacter.Get() == InPawn)
	{
		return;
	}
	CachedBaseCharacter = Cast<AOBaseCharacter>(InPawn);
	if (CachedBaseCharacter.IsValid() && CachedBaseCharacter->IsA<AOPlayerCharacter>())
	{
		AOPlayerCharacter* PlayerCharacter = Cast<AOPlayerCharacter>(CachedBaseCharacter.Get());
		EnableInput(this);
		if (PlayerCameraManager)
		{
			PlayerCameraManager->ViewPitchMin = PlayerCharacter->GetViewPitchMin();
			PlayerCameraManager->ViewPitchMax = PlayerCharacter->GetViewPitchMax();
		}
		WidgetManager->InitWidgets(this);
		WidgetManager->UnbindWidgets();
		WidgetManager->BindWidgets(PlayerCharacter);
	}
}

bool AOPlayerController::IsPressedAnyKeyForAction(const FName ActionName) const
{
	const TArray<FInputActionKeyMapping>& ArrayKeys = PlayerInput->GetKeysForAction(ActionName);
	bool bIsPressed = false;
	for (const auto ActionKey : ArrayKeys)
	{
		bIsPressed = bIsPressed || IsInputKeyDown(ActionKey.Key);
	}
	return bIsPressed;
}

bool AOPlayerController::FirstPressedKeyForAction(const FName ActionName, FText& OutKeyName) const
{
	const TArray<FInputActionKeyMapping>& ArrayKeys = PlayerInput->GetKeysForAction(ActionName);
	for (const auto ActionKey : ArrayKeys)
	{
		if (IsInputKeyDown(ActionKey.Key))
		{
			OutKeyName = ActionKey.Key.GetDisplayName();
			return true;
		}
	}
	return false;
}

void AOPlayerController::BindWidgets()
{
	WidgetManager->BindWidgets(CachedBaseCharacter.Get());
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
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &AOPlayerController::Mantle);
	InputComponent->BindAction("InteractionWithLadder", EInputEvent::IE_Pressed, this, &AOPlayerController::InteractionWithLadder);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AOPlayerController::Jump);
	InputComponent->BindAction("Crawl", EInputEvent::IE_Pressed, this, &AOPlayerController::ChangeCrawlState);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AOPlayerController::ChangeCrouchState);
	InputComponent->BindAction<FChangeSprint>("Sprint", EInputEvent::IE_Pressed, this, &AOPlayerController::ChangeSprint, true);
	InputComponent->BindAction<FChangeSprint>("Sprint", EInputEvent::IE_Released, this, &AOPlayerController::ChangeSprint, false);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AOPlayerController::StartFireWithWeapon);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &AOPlayerController::StopFire);
	InputComponent->BindAction("NextWeapon", EInputEvent::IE_Pressed, this, &AOPlayerController::NextWeapon);
	InputComponent->BindAction("NextWeaponIndex", EInputEvent::IE_Pressed, this, &AOPlayerController::NextWeaponIndex);
	InputComponent->BindAction("ReloadAmmo", EInputEvent::IE_Pressed, this, &AOPlayerController::ReloadAmmo);
	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &AOPlayerController::StartAiming);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &AOPlayerController::StopAiming);
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

void AOPlayerController::StartFireWithWeapon()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFire();
	}
}

void AOPlayerController::StopFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();
	}
}

void AOPlayerController::NextWeapon()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextWeapon();
	}
}

void AOPlayerController::ReloadAmmo()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ReloadAmmo();
	}
}

void AOPlayerController::NextWeaponIndex()
{
	if (CachedBaseCharacter.IsValid())
	{
		FText KeyName;
		if (FirstPressedKeyForAction(NextWeaponIndexActionName, KeyName))
		{
			FString KeyNameString = KeyName.ToString();
			if (KeyNameString.IsNumeric())
			{
				int32 NumberKey = FCString::Atoi(*KeyNameString);
				CachedBaseCharacter->NextWeaponIndex(NumberKey);
			}
		}
	}
}

void AOPlayerController::StartAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAiming();
	}
}

void AOPlayerController::StopAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAiming();
	}
}
