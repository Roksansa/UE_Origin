// Fill out your copyright notice in the Description page of Project Settings.


#include "OPrimaryAttributesComponent.h"

#include "OriginGameModeBase.h"
#include "Characters/OBaseCharacter.h"
#include "Utils/OBaseUtils.h"

UOPrimaryAttributesComponent::UOPrimaryAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOPrimaryAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	check(GetOwner());
	check(MaxHealth > 0);
	check(MaxStamina > 0);
	CurrentHealth = MaxHealth;
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UOPrimaryAttributesComponent::OnTakeAnyDamage);

	Character = Cast<AOBaseCharacter>(GetOwner());
	CurrentStamina = MaxStamina;
	if (MinStaminaForStartSprint > MaxStamina)
	{
		MinStaminaForStartSprint = MaxStamina;
	}
}

bool UOPrimaryAttributesComponent::IsFriendlyDamage(AController* InstigatedBy) const
{
	if (!Character.Get())
	{
		return false;
	}
	bool bOnFireFriendly = true;
	bool bIsEnemy = true;
	AOriginGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AOriginGameModeBase>();
	if (GameMode)
	{
		bOnFireFriendly = GameMode->IsOnFriendlyFire();
		if (!bOnFireFriendly)
		{
			bIsEnemy = OBaseUtils::AreTheyEnemies(InstigatedBy, Character.Get()->GetController());
		}
	}
	return !bOnFireFriendly && !bIsEnemy;
}

void UOPrimaryAttributesComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (IsFriendlyDamage(InstigatedBy))
	{
		return;
	}
	
	if (Damage > 0.f && CurrentHealth > SMALL_NUMBER)
	{
		const float PrevHealth = CurrentHealth;
		CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
		Damage = PrevHealth - CurrentHealth;
		if (CurrentHealth != PrevHealth)
		{
			OnChangeHealth.Broadcast(CurrentHealth, -Damage, MaxHealth);

			GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, FString::Printf(TEXT("Health: %.2f"), CurrentHealth), true,
				FVector2D(2, 2));
			if (CurrentHealth < SMALL_NUMBER)
			{
				GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Blue, FString::Printf(TEXT("Die:")), true,
					FVector2D(2, 2));

				if (AOriginGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AOriginGameModeBase>())
				{
					GameMode->UpdateKillDeathInfo(Character.Get() ? Character->GetController() : nullptr, InstigatedBy);
				}
				
				OnDie.Broadcast();
			}
		}
	}
}

void UOPrimaryAttributesComponent::TryChangeStamina(float DeltaSeconds, bool bIsSprinting)
{
	float PreviousStamina = CurrentStamina;
	//check if !bIsSprinting
	if (!bIsSprinting && CurrentStamina < MaxStamina)
	{
		CurrentStamina = FMath::Min(CurrentStamina + StaminaRestoreVelocity * DeltaSeconds, MaxStamina);
	}
	//check if bIsSprinting
	if (bIsSprinting && CurrentStamina > 0.0001f)
	{
		CurrentStamina = FMath::Max(CurrentStamina - SprintStaminaConsumptionVelocity * DeltaSeconds, 0.f);
	}

	//switch IsOutOfStamina after all calc
	//first - we can not start sprint - change and return
	if (!bIsOutOfStamina && CurrentStamina <= 0.0001f)
	{
		bIsOutOfStamina = true;
	}
	//second - we can start sprint - change and return
	if (bIsOutOfStamina && CurrentStamina >= MinStaminaForStartSprint)
	{
		bIsOutOfStamina = false;
	}

	if (CurrentStamina < MaxStamina)
	{
		const FColor CurrentColor = bIsOutOfStamina ? FColor::Red : FColor::Green;
		GEngine->AddOnScreenDebugMessage(1, 1.0f, CurrentColor, FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina), true,
			FVector2D(2, 2));
	}

	if (PreviousStamina != CurrentStamina)
	{
		OnChangeStamina.Broadcast(CurrentStamina, CurrentStamina - PreviousStamina, MaxStamina);
	}
}

bool UOPrimaryAttributesComponent::GetIsOutOfStamina() const
{
	return bIsOutOfStamina;
}

void UOPrimaryAttributesComponent::UpdateHealth() const
{
	OnChangeHealth.Broadcast(CurrentHealth, 0, MaxHealth);
}

void UOPrimaryAttributesComponent::UpdateStamina() const
{
	OnChangeStamina.Broadcast(CurrentStamina, 0, MaxStamina);
}

bool UOPrimaryAttributesComponent::TryAddHealth(int32 Value)
{
	if (Value > 0.f && CurrentHealth > SMALL_NUMBER && !FMath::IsNearlyEqual(CurrentHealth, MaxHealth))
	{
		const float PrevHealth = CurrentHealth;
		CurrentHealth = FMath::Clamp(CurrentHealth + Value, 0.f, MaxHealth);

		Value = CurrentHealth - PrevHealth;
		OnChangeHealth.Broadcast(CurrentHealth, Value, MaxHealth);
		return true;
	}
	return false;
}

bool UOPrimaryAttributesComponent::TryAddStamina(int32 Value)
{
	if (Value > 0.f && !FMath::IsNearlyEqual(CurrentStamina, MaxStamina))
	{
		const float PrevStamina = CurrentStamina;
		CurrentStamina = FMath::Clamp(CurrentStamina + Value, 0.f, MaxStamina);

		Value = CurrentStamina - PrevStamina;
		OnChangeStamina.Broadcast(CurrentStamina, Value, MaxStamina);
		return true;
	}
	return false;
}

bool UOPrimaryAttributesComponent::IsDead() const
{
	return CurrentHealth <= SMALL_NUMBER;
}

float UOPrimaryAttributesComponent::GetCurrentHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}

