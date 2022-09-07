#pragma once
#include "OTypes.h"

class UNiagaraComponent;
class UNiagaraSystem;

void SpawnVFX(const UWorld* InWorld, const FHitResult HitResult, const FOImpactData& DefaultEffect,
	const TMap<UPhysicalMaterial*, FOImpactData>& EffectsByPhys);

