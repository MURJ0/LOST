// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Treasure/Treasure.h"
#include "Interfaces/PickUpInterface.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSpherOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickUpInterface* PickUpInterface = Cast<IPickUpInterface>(OtherActor);
	if (PickUpInterface) {
		PickUpInterface->AddGold(this);
		if (CoinSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this, 
				CoinSound, 
				GetActorLocation());
		}
		Destroy();
	}
}
