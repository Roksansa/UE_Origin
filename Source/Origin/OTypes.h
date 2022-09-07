// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTypes.generated.h"

#define ECC_Climbing ECollisionChannel::ECC_GameTraceChannel1
#define ECC_InteractionVolume ECollisionChannel::ECC_GameTraceChannel2
#define ECC_InteractionObjectTrace ECollisionChannel::ECC_GameTraceChannel3
#define ECC_Firing ECollisionChannel::ECC_GameTraceChannel4

const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");


const FName DebugCategory_LedgeDetection("LedgeDetection");
const FName DebugCategory_IKFootCharacter("IKFootCharacter");

const FName MuzzleSocketName = FName("MuzzleSocket");
const FName WeaponSocketName = FName("WeaponSocket");
const FName ArmoryEquipSocketName = FName("ArmorySocket");

const FName SectionMontageStartBulletReload = FName("StartBulletReload");

const FName WeaponFireActionName = FName("Fire");
const FName NextWeaponIndexActionName = FName("NextWeaponIndex");
const FName OnHealthChangedName = FName("OnHealthChanged");
const FName AnimDiedName = FName("AnimDied");

const FName MenuLevelName = FName("Menu_Default");
const FName GymLevelName = FName("Gym_GameWithAI");

UENUM(BlueprintType)
enum class EOEquippableItemType : uint8
{
	None,
	Rifle,
	Launcher,
	Shotgun,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EOPrimaryAttr : uint8
{
	Health,
	Stamina,
	Die,
	MAX UMETA(Hidden)
};


UENUM(BlueprintType)
enum class EOBoostingType : uint8
{
	None,
	Health,
	Stamina,
	MAX UMETA(Hidden)
};

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct ORIGIN_API FODecalData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	UMaterialInterface* Material;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	FVector Size = FVector(1);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	float Lifetime = 3.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	float FadeOutTime = 0.5f;
};

USTRUCT(BlueprintType)
struct ORIGIN_API FOImpactData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* Effect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	FODecalData DecalData;
};

USTRUCT(BlueprintType)
struct ORIGIN_API FOGameData
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game", meta = (ClampMin = "1", ClampMax = "20"))
	int32 TeamNum = 2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game", meta = (ClampMin = "1", ClampMax = "20"))
	int32 PlayersNum = 2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game", meta = (ClampMin = "1", ClampMax = "5"))
	int32 RoundsNum = 2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game", meta = (ClampMin = "1", ClampMax = "100"))
	int32 RoundTimeInSec = 2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game", meta = (ClampMin = "1", ClampMax = "100"))
	int32 PlayerRespawnTimeInSec = 2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game", meta = (ClampMin = "2", ClampMax = "2"))
	TArray<FLinearColor> TeamColors;
};

UENUM(BlueprintType)
enum class EOMatchState : uint8
{
	WaitingToStart = 0,
	InProgress = 1,
	Pause = 2,
	GameOver = 3,
	MAX UMETA(Hidden)
};
