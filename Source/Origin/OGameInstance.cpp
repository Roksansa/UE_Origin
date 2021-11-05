// Fill out your copyright notice in the Description page of Project Settings.


#include "OGameInstance.h"

bool UOGameInstance::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool bResult = Super::ProcessConsoleExec(Cmd, Ar, Executor);
	if (!bResult)
	{
		TArray<UGameInstanceSubsystem*> Subsystems = GetSubsystemArray<UGameInstanceSubsystem>();
		for (auto Subsystem : Subsystems)
		{
			bResult |= Subsystem->ProcessConsoleExec(Cmd, Ar, Executor);
			if (bResult)
			{
				break;
			}
		}
	}
	return bResult;
}

void UOGameInstance::Init()
{
	Super::Init();
}

void UOGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UOGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
}
