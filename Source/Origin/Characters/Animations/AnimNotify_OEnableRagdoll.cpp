// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_OEnableRagdoll.h"

void UAnimNotify_OEnableRagdoll::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	MeshComp->SetCollisionProfileName(FName("Ragdoll"));
	MeshComp->SetSimulatePhysics(true);
	if (MeshComp->GetAnimInstance())
	{
		MeshComp->GetAnimInstance()->Montage_Stop(0.3f, nullptr);
	}
	Super::Notify(MeshComp, Animation);
}
