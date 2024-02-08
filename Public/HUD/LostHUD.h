// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LostHUD.generated.h"

class ULostOverlay;

/**
 * 
 */
UCLASS()
class LOSTV2_API ALostHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Lost)
	TSubclassOf<ULostOverlay> LostOverlayClass;

	UPROPERTY()
	ULostOverlay* LostOverlay;

public:
	FORCEINLINE ULostOverlay* GetLostOverlay() const { return LostOverlay; }
};
