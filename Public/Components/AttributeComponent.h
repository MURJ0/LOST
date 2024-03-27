// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOSTV2_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	virtual void BeginPlay() override;
	
private:
	// current Health
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	// current Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold;
	
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 15.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 5.f;

	float HealthRegenRateWhenResting = 3.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float EXP;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHP;

public:
	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaCost);

	float GetHealthPercent();
	float GetStaminaPercent();

	void AddGold(int32 AmountOfGold);
	void AddSouls(int32 AmountOfSouls);

	bool IsAlive();
	
	void RegenStamina(float DeltaTime);
	void RegenHealth(float DeltaTime);

	void AddEXP(float AddHP);

	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float GetEXP() const { return EXP; }

	void SetDodgeCost(float SetStaminaDodgeCost);
	void SetRegenRateCost(float SetStaminaRegenRate);
	void Heal();
};
