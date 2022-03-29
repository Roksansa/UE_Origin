// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OPrimaryAttributesComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChangeHealth, float, float, float);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChangeStamina, float, float, float);
DECLARE_MULTICAST_DELEGATE(FOnDie);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORIGIN_API UOPrimaryAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOPrimaryAttributesComponent();
	
	FOnChangeHealth OnChangeHealth;
	FOnDie OnDie;

	FOnChangeStamina OnChangeStamina;
	
	void TryChangeStamina(float DeltaSeconds, bool bIsSprinting);
	bool GetIsOutOfStamina() const;
	void UpdateHealth() const;
	void UpdateStamina() const;

	/**if need take damage - used owner's method OnTakeAnyDamage */
	bool TryAddHealth(int32 Value);
	bool TryAddStamina(int32 Value);
	bool IsDead() const;
	float GetCurrentHealthPercent() const;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting|Health", meta = (UIMin="0", UIMax="100", ClampMin="0", ClampMax="100.0"))
	float MaxHealth = 100.f;

	//begin stamina properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Setting|Stamina", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxStamina = 30.f; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Setting|Stamina", meta = (ClampMin = 0.f, UIMin = 0.f))
	float StaminaRestoreVelocity = 7.f; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Setting|Stamina", meta = (ClampMin = 0.f, UIMin = 0.f))
	float SprintStaminaConsumptionVelocity = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Setting|Stamina", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinStaminaForStartSprint = 30.f;

	bool bIsOutOfStamina;
	//end stamina properties
private:
	float CurrentHealth = 0.f;
	float CurrentStamina = 0.f;
};
