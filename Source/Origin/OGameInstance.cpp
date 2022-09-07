// Fill out your copyright notice in the Description page of Project Settings.


#include "OGameInstance.h"

#include "MoviePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/OBaseBlueprintFunctionLibrary.h"

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
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UOGameInstance::BeginLoadingScreen);
}

void UOGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UOGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
}

void UOGameInstance::BeginLoadingScreen(const FString& MapName)
{
	UOBaseBlueprintFunctionLibrary::PlayLoadingScreen();
	
	if (GEngine)
	{
		Handler = GetMoviePlayer()->OnMoviePlaybackFinished().AddUObject(this, &UOGameInstance::EndLoadingScreen);
	}
}

void UOGameInstance::EndLoadingScreen()
{
	if (GEngine)
	{
		GetMoviePlayer()->OnMoviePlaybackFinished().Remove(Handler);
		APlayerCameraManager* const Camera = UGameplayStatics::GetPlayerCameraManager(this, 0);
		if (Camera)
		{
			Camera->StartCameraFade(1.f, 0.f, 1.f, FLinearColor::Black);
		}
	}
}
