// Fill out your copyright notice in the Description page of Project Settings.


#include "OBaseBlueprintFunctionLibrary.h"
#include "OriginLoadingScreen.h"

void UOBaseBlueprintFunctionLibrary::PlayLoadingScreen()
{
	IOriginLoadingScreenModule& LoadingScreenModule = IOriginLoadingScreenModule::Get();
	LoadingScreenModule.StartInGameLoadingScreen();
}

void UOBaseBlueprintFunctionLibrary::StopLoadingScreen()
{
	IOriginLoadingScreenModule& LoadingScreenModule = IOriginLoadingScreenModule::Get();
	LoadingScreenModule.StopInGameLoadingScreen();
}
