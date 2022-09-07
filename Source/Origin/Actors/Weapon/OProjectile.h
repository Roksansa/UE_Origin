// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OLauncherWeapon.h"
#include "GameFramework/Actor.h"
#include "OProjectile.generated.h"


class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ORIGIN_API AOProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AOProjectile();

	void SetShotDirection(const FVector& Vector);
	void SetDamageAmount(float Value);
	void SetNiagaraEffect(const FOImpactData& DefaultImpactData, const TMap<UPhysicalMaterial*, FOImpactData>& ImpactsMap);
protected:

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	UProjectileMovementComponent* MovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float DamageRadius = 200.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool DoFullDamage = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float LifeSeconds = 5.f;
	virtual void BeginPlay() override;

private:
	FVector ShotDirection;
	
	float DamageAmount = 70.f;
	
	FOImpactData DefaultSpawnEffect;

	const TMap<UPhysicalMaterial*, FOImpactData>* SpawnEffects;
	
	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
