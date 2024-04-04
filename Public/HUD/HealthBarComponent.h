// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class LOSTV2_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void SetHealthPercent(float Percent);
	void SetDamageRecieved(float Damage);

	void ResetDamageText();
private:
	UPROPERTY()
	class UHealthBar* HealthBarWidget;

	FTimerHandle TimerHandle_ResetDamageText;
};
