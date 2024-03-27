#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LostOverlay.generated.h"

UCLASS()
class LOSTV2_API ULostOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetEXPBarPercent(float Percent);

	void SetGold(int32 Gold);
	void SetSouls(int32 Souls);

private:

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* EXPProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulText;
};
