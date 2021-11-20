// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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

UENUM(BlueprintType)
enum class EOEquippableItemType : uint8
{
	None,
	Rifle,
	Launcher,
	Shotgun,
	MAX UMETA(Hidden)
};