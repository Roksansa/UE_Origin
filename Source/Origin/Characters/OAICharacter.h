// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/OBaseCharacter.h"
#include "OAICharacter.generated.h"

class UBehaviorTree;

UCLASS(Blueprintable)
class ORIGIN_API AOAICharacter : public AOBaseCharacter
{
	GENERATED_BODY()

public:
	AOAICharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void ReloadAmmo() override;
	virtual void NextWeapon() override;
	bool TryChangeOrReload(bool bCheckAmmoEmpty = false);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AI")
	UBehaviorTree* BehaviorTree;

	const UOPrimaryAttributesComponent* GetPrimaryAttributesComponent() const;
protected:
	virtual void OnDie() override;
};
