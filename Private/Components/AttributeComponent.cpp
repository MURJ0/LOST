// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributeComponent::Heal()
{
	Health += 40.f;
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

void UAttributeComponent::AddXP(float AddEXP)
{
	if (XP + AddEXP >= MaxXP) {
		float currEXP = (XP + AddEXP) - MaxXP;
		XP = currEXP;
		MaxXP += 25.f;
		MaxHealth += 25.f;
		MaxStamina += 25.f;
		AddLevel();
		return;
	}

	XP = FMath::Clamp(XP + AddEXP, 0.f, MaxXP);
}

void UAttributeComponent::AddLevel()
{
	Level = FMath::Clamp(Level + 1, 0.f, MaxLevel);
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina;
}

float UAttributeComponent::GetXPPercent()
{
	return XP / MaxXP;
}

void UAttributeComponent::AddGold(int32 AmountOfGold)
{
	Gold += AmountOfGold;
}

void UAttributeComponent::AddSouls(int32 AmountOfSouls)
{
	Souls += AmountOfSouls;
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

void UAttributeComponent::RegenHealth(float DeltaTime)
{
	Health = FMath::Clamp(Health + HealthRegenRateWhenResting * DeltaTime, 0.f, MaxHealth);
}

void UAttributeComponent::AddDamage(float Damage)
{
	MinDamage += Damage;
	MaxDamage += Damage;
}


void UAttributeComponent::SetDodgeCost(float SetStaminaDodgeCost)
{
	DodgeCost = SetStaminaDodgeCost;
}

void UAttributeComponent::SetRegenRateCost(float SetStaminaRegenRate)
{
	StaminaRegenRate = SetStaminaRegenRate;
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

