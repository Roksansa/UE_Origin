// Fill out your copyright notice in the Description page of Project Settings.


#include "OBasePickup.h"

#include "Characters/OBaseCharacter.h"
#include "Components/SphereComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogPawn, Warning, All);
// Sets default values
AOBasePickup::AOBasePickup()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->InitSphereRadius(30.f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SetRootComponent(CollisionComponent);

}

bool AOBasePickup::IsVisibleActor() const
{
	return !bTakenPickup;
}

// Called when the game starts or when spawned
void AOBasePickup::BeginPlay()
{
	Super::BeginPlay();
	bTakenPickup = false;
	check(CollisionComponent);
}

void AOBasePickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	AOBaseCharacter* BaseCharacter = Cast<AOBaseCharacter>(OtherActor);
	if (BaseCharacter && !bTakenPickup && GivePickupTo(BaseCharacter))
	{
		TakePickup();
		UE_LOG(LogPawn, Display, TEXT("Pickup was taken"));
	}
}

void AOBasePickup::TakePickup()
{
	bTakenPickup = true;
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetRootComponent()->SetVisibility(false, true);
	FTimerHandle RespawnTimer;
	GetWorldTimerManager().SetTimer(RespawnTimer, this, &AOBasePickup::Respawn, RespawnTime);
	//todo послать пикап
}

void AOBasePickup::Respawn()
{
	bTakenPickup = false;
	GetRootComponent()->SetVisibility(true, true);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

bool AOBasePickup::GivePickupTo(AOBaseCharacter* Character)
{
	return false;
}


