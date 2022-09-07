// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

/** Module interface for this game's loading screens */
class IOriginLoadingScreenModule : public IModuleInterface
{
public:
	static inline IOriginLoadingScreenModule& Get()
	{
		return FModuleManager::LoadModuleChecked<IOriginLoadingScreenModule>("OriginLoadingScreen");
	}

	virtual void StartInGameLoadingScreen() = 0;

	virtual void StopInGameLoadingScreen() = 0;
};
