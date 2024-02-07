// Fill out your copyright notice in the Description page of Project Settings.
#include "Characters/LostAnimInstance.h"
#include "Characters/LostV2Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

// QJ kur ceco!!!
// AMA NAJ MNOGO GO QDE STIVO !!!







void ULostAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	LostCharacter = Cast<ALostV2Character>(TryGetPawnOwner());
	if (LostCharacter) {
		LostCharacterMovement = LostCharacter->GetCharacterMovement();
	}
}


void ULostAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (LostCharacterMovement) {
		GroundSpeed = UKismetMathLibrary::VSizeXY(LostCharacterMovement->Velocity);
		IsFalling = LostCharacterMovement->IsFalling();
		CharacterState = LostCharacter->GetCharacterState();
	}
}
