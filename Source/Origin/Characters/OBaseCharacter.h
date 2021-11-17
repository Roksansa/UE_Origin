// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Settings/OMantlingSettings.h"

#include "OBaseCharacter.generated.h"

class UOBaseCharacterMovementComponent;
class AOInteractiveActor;
class UOCharacterIKComponent;
class UOLedgeDetectorComponent;
class AOLadderInteractiveActor;
class UOWeaponComponent;
class UOPrimaryAttributesComponent;

UCLASS(Abstract, NotBlueprintable)
class ORIGIN_API AOBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AOBaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void LookUp(float Value) {}
	virtual void Turn(float Value) {}
	virtual void MoveRight(float Value) {};
	virtual void MoveForward(float Value) {};

	virtual void TurnAtRate(float Value) {};
	virtual void LookUpAtRate(float Value) {};

	virtual void ChangeCrouchState();
	virtual void ChangeCrawlState();
	virtual void ChangeSprint(bool bWantsToSprint);
	virtual void Jump() override;

	virtual void MoveSwimRight(float Value) {};
	virtual void MoveSwimForward(float Value, float LastSwimUpValue) {};
	virtual void MoveSwimUp(float Value) {};
	
	virtual void InteractionWithLadder(){};
	virtual void ClimbLadder(float Value) {};
	
	virtual void StartFire();
	virtual void StopFire();
	
	UOBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const;

	UOCharacterIKComponent* GetIKComponent() const;

	UPROPERTY(BlueprintReadOnly, Category = Character)
	uint32 bIsSprinting:1;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character|Input")
    void OnSprintStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Character|Input")
    void OnSprintEnd();

	virtual void Tick(float DeltaSeconds) override;

	bool GetIsOutOfStamina() const;

	bool GetIsCrawling() const;
	void SetIsCrawling(bool bCrawl);
	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
	virtual void UnCrawl(bool bClientSimulation = false);
	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
	virtual void Crawl(bool bClientSimulation = false);
	virtual void OnStartCrawling(float HalfHeightAdjust, float ScaledHeightAdjust);
	virtual void OnEndCrawling(float HalfHeightAdjust, float ScaledHeightAdjust);
	virtual void OnStartSwimming(float HalfHeightAdjust, float ScaledHeightAdjust);
	virtual void OnEndSwimming(float HalfHeightAdjust, float ScaledHeightAdjust);

	virtual void Mantle();

	virtual void NextWeapon();

	bool GetIsOverlapVolumeSurface() const;
	UFUNCTION(BlueprintCallable)
	void ChangeBuoyancyFromSurfaceVolume(bool bIsOverlapVolumeSurfaceNow);

	void RegisterInteractiveActor(AOInteractiveActor* InterActor);
	void UnregisterInteractiveActor(AOInteractiveActor* InterActor);

	const UOWeaponComponent* GetWeaponComponent() const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Input")
	float BaseTurnRate = 45.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Input")
	float BaseLookUpRate = 45.f;
	
	UPROPERTY()
	UOBaseCharacterMovementComponent* BaseCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UOCharacterIKComponent* IKComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UOWeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UOPrimaryAttributesComponent* PrimaryAttributesComponent;

	virtual bool CanSprint();

	UPROPERTY(BlueprintReadOnly, Category = "Config|Crawling")
	uint32 bIsCrawling:1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config|Swimming")
	bool bIsOverlapVolumeSurface = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Swimming", meta = (ClampMin = 1.1f, UIMin = 1.1f, ClampMax = 50.0f, UIMax = 50.0f))
	float BuoyancyInWaterSurface = 1.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config|Movement")
	UOLedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Movement|Mantling")
	FOMantlingSettings HighMantlingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Movement|Mantling")
	FOMantlingSettings LowMantlingSettings;

	UPROPERTY()
	TArray<AOInteractiveActor*> AvailableInteractiveActors;

	const AOLadderInteractiveActor* GetAvailableLadder() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Anim")
	UAnimMontage* DeathAnimMontage;
	UFUNCTION()
	void OnDie();

	UFUNCTION()
	void OnChangeHealth(float Health, float Diff);
private:
	void FillMantlingMovementParameters(struct FLedgeDescription LedgeDescription, FMantlingMovementParameters& MantlingMovementParameters) const;
	const FOMantlingSettings& GetMantlingSettings(float LedgeHeight) const;
};
