// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/OBaseCharacter.h"
#include "Components/ActorComponent.h"
#include "OWidgetManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORIGIN_API UOWidgetManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOWidgetManagerComponent();

	void BindWidgets(AOBaseCharacter* Character);
	void InitWidgets();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget")
	TSubclassOf<class UOMainWidget> MainWidgetTemplate;

private:
	bool bIsWidgetsBinded = false;

	TWeakObjectPtr<class UOMainWidget> MainWidget;
};
