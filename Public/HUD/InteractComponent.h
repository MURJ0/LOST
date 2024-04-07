// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractComponent.generated.h"

/**
 * 
 */
UCLASS()
class LOSTV2_API UInteractComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void SetInteractText(FString Text);
	void HideInteractText();
	void ShowInteractText();

private:
	UPROPERTY()
	class UInteractOverlay* InteractWidget;
};
