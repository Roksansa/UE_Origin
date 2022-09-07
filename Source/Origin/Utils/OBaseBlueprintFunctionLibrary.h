// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OGameInstance.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OBaseBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UOBaseBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = Loading)
	static void PlayLoadingScreen();

	UFUNCTION(BlueprintCallable, Category = Loading)
	static void StopLoadingScreen();
};
