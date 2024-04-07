// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Bonfire.generated.h"

/**
 * 
 */
UCLASS()
class LOSTV2_API ABonfire : public AItem
{
	GENERATED_BODY()
	

public:
	ABonfire();

	virtual void Tick(float DeltaTime) override;

	virtual void OnSpherOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)override ;

	void ActivateBonfire();

	bool IsBonfireActive() const { return bActive; }

	FVector GetBonfireLocation() const { return BonfireLocation; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector BonfireLocation;

	bool bActive = false;
};
