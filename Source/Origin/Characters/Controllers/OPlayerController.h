// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTypes.h"
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
	AOPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void SetPawn(APawn* InPawn) override;
	bool IsPressedAnyKeyForAction(const FName ActionName) const;
	bool FirstPressedKeyForAction(const FName ActionName, FText& OutKeyName) const;

	void BindWidgets();
	FPawnChangedSignature& GetOnSetPawnNotifier() { return OnSetPawn; }
protected:
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	UPROPERTY(VisibleAnywhere)
	class UOWidgetManagerComponent* WidgetManager;
	FPawnChangedSignature OnSetPawn;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="GameLogic")
	class UOPlayerRespawnComponent* RespawnComponent;
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
	void StartAiming();
	void StopAiming();
	void PauseGame();
	void ResetGame();
	void GoToMenu();

	// gimbal lock for swim up and swim forward for y = 90. if rotate vector up to forward vector
	float LastSwimUpValue = 0.f;

	void UpdateMatchState(EOMatchState MatchState);
};
