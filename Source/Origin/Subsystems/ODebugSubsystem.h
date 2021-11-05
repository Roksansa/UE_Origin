// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ODebugSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UODebugSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	bool IsCategoryEnabled(const FName& CategoryName) const;

private:
	UFUNCTION(exec)
	void EnableDebugCategory(const FName& CategoryName, bool bIsEnable);

	UPROPERTY()
	TMap<FName, bool> EnableDebugCategories;
};
