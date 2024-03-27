#include "HUD/LostOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ULostOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar) {
		HealthProgressBar->SetPercent(Percent);
	}
}

void ULostOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar) {
		StaminaProgressBar->SetPercent(Percent);
	}
}

void ULostOverlay::SetGold(int32 Gold)
{
	if (GoldText) {
		GoldText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Gold)));
	}
}

void ULostOverlay::SetSouls(int32 Souls)
{
	if (SoulText) {
		SoulText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Souls)));
	}
}

void ULostOverlay::SetEXPBarPercent(float Percent)
{
	if (EXPProgressBar) {
		EXPProgressBar->SetPercent(Percent);
	}
}


