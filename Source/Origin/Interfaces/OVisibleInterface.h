// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OVisibleInterface.generated.h"

UINTERFACE()
class ORIGIN_API UOVisibleInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ORIGIN_API IOVisibleInterface
{
	GENERATED_IINTERFACE_BODY()

	virtual bool IsVisibleActor() const { return false; }
};
