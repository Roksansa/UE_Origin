#include "OSpawnUtils.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "OTypes.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

void SpawnVFX(const UWorld* InWorld, const FHitResult HitResult, const FOImpactData& DefaultEffect,
	const TMap<UPhysicalMaterial*, FOImpactData>& EffectsByPhys)
{
	FOImpactData Effect = DefaultEffect;
	if (HitResult.PhysMaterial.IsValid())
	{
		const UPhysicalMaterial* PhysMat = HitResult.PhysMaterial.Get();
		if (EffectsByPhys.Contains(PhysMat))
		{
			Effect = EffectsByPhys[PhysMat];
		}
	}
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(InWorld, Effect.Effect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
	UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(InWorld, Effect.DecalData.Material, Effect.DecalData.Size,
		HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
	if (Decal)
	{
		Decal->SetFadeScreenSize(0.001f);
		Decal->SetFadeOut(Effect.DecalData.Lifetime, Effect.DecalData.FadeOutTime);
	}
}
