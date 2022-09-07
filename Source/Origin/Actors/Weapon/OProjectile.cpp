// Fill out your copyright notice in the Description page of Project Settings.


#include "OProjectile.h"

#include "DrawDebugHelpers.h"
#include "NiagaraSystem.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Utils/OSpawnUtils.h"

// Sets default values
AOProjectile::AOProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->InitSphereRadius(5.f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionComponent->bReturnMaterialOnMove = true;
	SetRootComponent(CollisionComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->ProjectileGravityScale = 0.3f;
	MovementComponent->InitialSpeed = 2222.f;
}

void AOProjectile::BeginPlay()
{
	Super::BeginPlay();
	check(MovementComponent);
	check(CollisionComponent);
	MovementComponent->Velocity = ShotDirection * MovementComponent->InitialSpeed;
	CollisionComponent->OnComponentHit.AddDynamic(this, &AOProjectile::OnProjectileHit);
	CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
	SetLifeSpan(LifeSeconds);
}

void AOProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!GetWorld())
	{
		return;
	}
	MovementComponent->StopMovementImmediately();
	CollisionComponent->IgnoreActorWhenMoving(GetOwner(), false);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), DamageAmount, GetActorLocation(), DamageRadius, UDamageType::StaticClass(), {},
		this, GetInstigatorController(), DoFullDamage);
	DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 24, FColor::Cyan, false, 3.f);
	SpawnVFX(GetWorld(), Hit, DefaultSpawnEffect, *SpawnEffects);
	Destroy();
}

void AOProjectile::SetShotDirection(const FVector& Vector)
{
	ShotDirection = Vector;
}

void AOProjectile::SetDamageAmount(float Value)
{
	DamageAmount = Value;
}

void AOProjectile::SetNiagaraEffect(const FOImpactData& DefaultImpactData, const TMap<UPhysicalMaterial*, FOImpactData>& ImpactsMap)
{
	DefaultSpawnEffect = DefaultImpactData;
	SpawnEffects = &ImpactsMap;
}
