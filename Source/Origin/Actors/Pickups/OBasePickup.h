// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OBasePickup.generated.h"

UCLASS()
class ORIGIN_API AOBasePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	AOBasePickup();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class USphereComponent* CollisionComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float RespawnTime = 10.f;

	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
	void TakePickup();
	void Respawn();
	virtual bool GivePickupTo(class AOBaseCharacter* Character);
};
