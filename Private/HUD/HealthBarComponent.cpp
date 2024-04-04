// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	if (HealthBarWidget == nullptr) {
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}

	if (HealthBarWidget && HealthBarWidget->HealthBar) {
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}
}

void UHealthBarComponent::SetDamageRecieved(float Damage)
{
	if (HealthBarWidget == nullptr) {
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}

	if (HealthBarWidget && HealthBarWidget->DamageText)
	{
		int32 DamageInt = FMath::RoundToInt(Damage); // Convert float to integer
		HealthBarWidget->DamageText->SetText(FText::FromString(FString::Printf(TEXT("%d"), DamageInt)));

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ResetDamageText, this, &UHealthBarComponent::ResetDamageText, 1.0f, false);
	}
	
}

void UHealthBarComponent::ResetDamageText()
{
	if (HealthBarWidget && HealthBarWidget->DamageText)
	{
		// Reset damage text
		HealthBarWidget->DamageText->SetText(FText::FromString(TEXT("")));
	}
}