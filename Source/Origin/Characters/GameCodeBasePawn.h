// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameCodeBasePawn.generated.h"

UCLASS()
class ORIGIN_API AGameCodeBasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGameCodeBasePawn();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere)
	class UPawnMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;
	
	void Jump();
	
	UFUNCTION(BlueprintPure)
    float GetInputForward() const { return InputForward;}

	UFUNCTION(BlueprintPure)
    float GetInputRight() const { return InputRight;}
protected:
	
	void MoveForward(float Value);
	
	void MoveRight(float Value);

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Config | BasePawn")
	float CollisionSphereRadius = 50.f;
private:
	UFUNCTION()
	void OnBlendComplete();

	UPROPERTY(Transient)
	AActor* CurrentViewActor;

	float InputForward = 0.f;
	
	float InputRight = 0.f;
};
