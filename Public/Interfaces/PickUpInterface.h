// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickUpInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickUpInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LOSTV2_API IPickUpInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetOverlappingItem(class AItem* Item) = 0;
	virtual void AddSouls(class ASoul* Soul) = 0;
	virtual void AddGold(class ATreasure* Gold) = 0;
};
