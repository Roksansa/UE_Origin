// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ORIGIN_API UOGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void StartGameInstance() override;
};
