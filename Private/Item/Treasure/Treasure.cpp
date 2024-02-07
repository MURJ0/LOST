// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Treasure/Treasure.h"
#include "Characters/LostV2Character.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSpherOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ALostV2Character* LostCharacter = Cast<ALostV2Character>(OtherActor);
	if (LostCharacter) {
		if (CoinSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, CoinSound, GetActorLocation());
		}
		Destroy();
	}
}
