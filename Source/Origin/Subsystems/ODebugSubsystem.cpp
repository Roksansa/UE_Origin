// Fill out your copyright notice in the Description page of Project Settings.


#include "ODebugSubsystem.h"

bool UODebugSubsystem::IsCategoryEnabled(const FName& CategoryName) const
{
	const bool* bIsEnabled = EnableDebugCategories.Find(CategoryName);
	return bIsEnabled != nullptr && *bIsEnabled;
}

void UODebugSubsystem::EnableDebugCategory(const FName& CategoryName, bool bIsEnable)
{
	EnableDebugCategories.FindOrAdd(CategoryName);
	EnableDebugCategories[CategoryName] = bIsEnable;
}
