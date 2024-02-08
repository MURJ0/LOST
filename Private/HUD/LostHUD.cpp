#include "HUD/LostHUD.h"
#include "HUD/LostOverlay.h"

void ALostHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World) {
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && LostOverlayClass) {
			LostOverlay = CreateWidget<ULostOverlay>(Controller, LostOverlayClass);
			LostOverlay->AddToViewport();
		}
	}

}
