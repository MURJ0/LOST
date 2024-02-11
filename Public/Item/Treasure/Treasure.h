// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class LOSTV2_API ATreasure : public AItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundWave* CoinSound;

	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	int32 Gold;
	
protected:
	void OnSpherOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
public:
	FORCEINLINE int32 GetGold() const { return Gold; }
};
