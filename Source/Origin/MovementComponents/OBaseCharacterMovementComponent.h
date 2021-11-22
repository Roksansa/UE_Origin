// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Origin/Characters/Settings/OMantlingSettings.h"
#include "OBaseCharacterMovementComponent.generated.h"

UENUM()
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_ClimbLadder UMETA(DisplayName = "ClimbLadder"),
	CMOVE_MAX UMETA(Hidden)
};
/**
 * 
 */
UCLASS()
class ORIGIN_API UOBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	//begin crouch settings
	virtual bool CanCrouchInCurrentState() const override;
	bool CanGrowCapsule(bool bClientSimulation = false) const;
	virtual void UnCrouch(bool bClientSimulation = false) override;
	virtual void Crouch(bool bClientSimulation = false) override;
	//end crouch settings

	//begin spring settings
	bool CanSprint() const;
	bool IsSprinting() const;
	void ChangeSprint(bool bStartSprint);
	bool CanSprintInCurrentState() const;
	/** If true, try to Sprint (or keep Sprinting) on next update. If false, try to stop Sprint on next update. */
	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToSprint:1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Movement: Walking" , meta=(ClampMin="0", UIMin="0"))
	float SprintingMaxSpeedWalking = 900.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Movement: Walking" , meta=(ClampMin="0", UIMin="0"))
	float SprintingMaxSpeedCrouch = 400.f;
	//end spring settings
	//override block for work with add mechanics
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
	virtual bool ShouldRemainVertical() const override;
	virtual void PhysicsRotation(float DeltaTime) override;

	virtual float GetMaxSpeed() const override;
	
	//begin stamina settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Movement: Walking" , meta=(ClampMin="0", UIMin="0"))
	float MaxSpeedOutOfStamina = 300.f;	
	bool IsOutOfStamina() const;
	//end stamina settings

	//begin crawl settings
	bool IsCrawling() const;
	/** If true, try to Crawl (or keep Crawling) on next update. If false, try to stop Crawl on next update. */
    UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
    uint8 bWantsToCrawling:1;
	bool CanCrawlingInCurrentState() const;
	virtual void StartCrawl();
	virtual void StopCrawl();
	//end crawl

	virtual void StartMantle(const FOMantlingMovementParameters& MantlingMovementParameters);
	virtual void StopMantle();
	bool IsMantling() const;

	virtual void AttachLadder(const class AOLadderInteractiveActor* Ladder);
	float GetProjectionFromActorToLadder(const FVector& CurrentLocation) const;
	virtual void DetachFromLadder(bool bWithJump = true);
	bool IsClimbingLadder() const;
	const AOLadderInteractiveActor* GetCurrentLadder() const;
	float GetClimbingLadderSpeedRatio() const;
	float GetLadderToCharacterOffset() const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Movement: Crawl" , meta=(ClampMin="0", UIMin="0"))
	float MaxCrawlSpeed = 100.0f;
	/** Collision half-height when crouching (component scale is applied separately) */
	UPROPERTY(Category="Character Movement: Crawl", EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", UIMin="0"))
	float CrawlingHalfHeight = 20.f;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	virtual void PhysMantling(float DeltaTime, int32 Iterations);
	virtual void PhysClimbLadder(float DeltaTime, int32 Iterations);
	void BaseCharacterOnLadderPhysRotation(float DeltaTime);
	
	UPROPERTY(Category="Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float SwimmingCapsuleHalfSize = 50.f;
	UPROPERTY(Category="Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float SwimmingRadiusSize = 50.f;
	UPROPERTY(Category="Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float SprintingMaxSpeedSwimming = 450.f;
	UPROPERTY(Category="Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float CoeffSpeedSwimmingOnSurface = 0.8f;

	UPROPERTY(Category="Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float ClimbingLadderMaxSpeed = 300.f;
	UPROPERTY(Category="Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float SprintingClimbingLadderMaxSpeed = 300.f;
	UPROPERTY(Category="Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float ClimbingLadderBrakingDeceleration = 2048.f;
	UPROPERTY(Category="Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float LadderToCharacterOffset = 60.f;
	UPROPERTY(Category="Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float LadderMinBottomOffset = 80;
	UPROPERTY(Category="Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float LadderMaxTopOffset = 60;
	UPROPERTY(Category="Character Movement: Ladder", VisibleAnywhere, BlueprintReadOnly)
	FRotator ToLadderRotator = FRotator::ZeroRotator;
	
	UPROPERTY(Category="Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float MaxWalkWithWeaponSpeed = 60;
private:
	//begin additional params for spring settings
	UPROPERTY(Transient, DuplicateTransient)
	class AOBaseCharacter* CachedBaseCharacter;
	//end spring settings

	FOMantlingMovementParameters CurrentMantlingParameters;
	FTimerHandle MantlingTimer;

	UPROPERTY()
	const class AOLadderInteractiveActor* CurrentLadder = nullptr;
	bool bCrossLadderMinBottomOffset = false;
	FVector ActorPositionOnLadderWorldSpace = FVector::ZeroVector;

	bool bRotateToLadder = false;
};
