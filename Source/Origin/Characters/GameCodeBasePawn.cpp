// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCodeBasePawn.h"

#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"
#include "Origin/MovementComponents/BasePawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGameCodeBasePawn::AGameCodeBasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
    CollisionSphereRadius = 50.f;
	CollisionComponent->SetSphereRadius(CollisionSphereRadius);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = CollisionComponent;
	

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UBasePawnMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->SetUpdatedComponent(CollisionComponent);
}

// Called to bind functionality to input
void AGameCodeBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGameCodeBasePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGameCodeBasePawn::MoveRight);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &AGameCodeBasePawn::Jump);
}

void AGameCodeBasePawn::Jump()
{
	checkf(MovementComponent->IsA<UBasePawnMovementComponent>(), TEXT("AGameCodeBasePawn::Jump() work with only UBasePawnMovementComponent"));
	UBasePawnMovementComponent* BaseMovementComponent = StaticCast<UBasePawnMovementComponent*>(MovementComponent);
	BaseMovementComponent->JumpStart();
}

void AGameCodeBasePawn::MoveForward(float Value)
{
	InputForward = Value;
	if (Value != 0.f)
	{
		AddMovementInput(CurrentViewActor->GetActorForwardVector(), Value);
	}
}

void AGameCodeBasePawn::MoveRight(float Value)
{
	InputRight = Value;
	if (Value != 0.f)
	{
		AddMovementInput(CurrentViewActor->GetActorRightVector(), Value);
	}
}

void AGameCodeBasePawn::BeginPlay()
{
	Super::BeginPlay();
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	CurrentViewActor = CameraManager->GetViewTarget();
	CameraManager->OnBlendComplete().AddUFunction(this, FName("OnBlendComplete"));
}

void AGameCodeBasePawn::OnBlendComplete()
{
	CurrentViewActor = GetController()->GetViewTarget();
}

