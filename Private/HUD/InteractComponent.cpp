// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/InteractOverlay.h"
#include "HUD/InteractComponent.h"
#include "Components/TextBlock.h"

void UInteractComponent::SetInteractText(FString Text)
{
	if (InteractWidget == nullptr) {
		InteractWidget = Cast<UInteractOverlay>(GetUserWidgetObject());
	}

	if (InteractWidget && InteractWidget->InteractText)
	{
		InteractWidget->InteractText->SetText(FText::FromString(Text));
	}
}

void UInteractComponent::HideInteractText()
{
	if (InteractWidget && InteractWidget->InteractText)
	{
		InteractWidget->InteractText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInteractComponent::ShowInteractText()
{
	if (InteractWidget && InteractWidget->InteractText)
	{
		InteractWidget->InteractText->SetVisibility(ESlateVisibility::Visible);
	}
}
