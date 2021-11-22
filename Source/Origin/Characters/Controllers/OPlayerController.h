// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OPlayerController.generated.h"

/**
 * 
 */

DECLARE_DELEGATE_OneParam(FChangeSprint, bool);

UCLASS()
class ORIGIN_API AOPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    virtual void SetPawn(APawn* InPawn) override;
	bool IsPressedAnyKeyForAction(const FName ActionName) const;
	bool FirstPressedKeyForAction(const FName ActionName, FText& OutKeyName) const;

protected:
    virtual void SetupInputComponent() override;
    
private:
    TWeakObjectPtr<class AOBaseCharacter> CachedBaseCharacter;
    void ChangeSprint(bool bWantsToSprint);
    void LookUp(float Value);
    void Turn(float Value);
    void MoveRight(float Value);
    void MoveForward(float Value);
    void LookUpAtRate(float Value);
    void TurnAtRate(float Value);
    void Jump();
    void ChangeCrouchState();
    void ChangeCrawlState();
    void MoveSwimRight(float Value);
    void MoveSwimForward(float Value);
    void MoveSwimUp(float Value);

    void Mantle();
    void ClimbLadder(float Value);
    void InteractionWithLadder();

	void StartFireWithWeapon();
	void StopFire();
	void NextWeapon();
	void ReloadAmmo();
	void NextWeaponIndex();

    // gimbal lock for swim up and swim forward for y = 90. if rotate vector up to forward vector
    float LastSwimUpValue = 0.f;
};
