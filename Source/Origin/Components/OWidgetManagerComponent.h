// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/OBaseCharacter.h"
#include "Components/ActorComponent.h"
#include "OWidgetManagerComponent.generated.h"

class UOMainWidget;
class AOPlayerController;
class AOriginPlayerState;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORIGIN_API UOWidgetManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOWidgetManagerComponent();

	void BindWidgets(AOBaseCharacter* Character);
	void InitWidgets(AController* NewController);
	void UnbindWidgets();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget")
	TSubclassOf<UOMainWidget> MainWidgetTemplate;

private:
	bool bIsWidgetsBinded = false;

	TWeakObjectPtr<UOMainWidget> MainWidget;
	TWeakObjectPtr<AOPlayerController> Controller;
	TWeakObjectPtr<AOriginPlayerState> PlayerState;

	void UpdateDesc(const AController* DeadController, const AController* KillerController, int32 Death, int32 Kills);
	void ShowAllStats();
	void UpdateMatchState(EOMatchState MatchState);
};
