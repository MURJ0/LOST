// Fill out your copyright notice in the Description page of Project Settings.
#include "Characters/LostAnimInstance.h"
#include "Characters/LostV2Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		ActionState = LostCharacter->GetActionState();
		LostDeathPose = LostCharacter->GetLostDeathPose();

		if (LostCharacter->IsCharacterLockedOnTheClosestEnemy()) {
			UpdateCharacterDirection();
		}
	}
}

void ULostAnimInstance::UpdateCharacterDirection()
{
	// Store the movement direction based on ForwardDotProduct and RightDotProduct
	if (ForwardDotProduct > 0.5f) {
		UE_LOG(LogTemp, Warning, TEXT("Forward direction"));
		//MovementDirection = ForwardDotProduct; // Set MovementDirection based on ForwardDotProduct
		MovementDirection = +1.0f;
	}
	else if (ForwardDotProduct < -0.5f) {
		UE_LOG(LogTemp, Warning, TEXT("Backward direction"));
		//MovementDirection = ForwardDotProduct; // Set MovementDirection based on ForwardDotProduct
		MovementDirection = -1.0f;
	}
	else if (RightDotProduct > 0.5f) {
		UE_LOG(LogTemp, Warning, TEXT("Right direction"));
		//MovementDirection = RightDotProduct; // Set MovementDirection based on RightDotProduct
		MovementDirection = -2.0f;
	}
	else if (RightDotProduct < -0.5f) {
		UE_LOG(LogTemp, Warning, TEXT("Left direction"));
		//MovementDirection = RightDotProduct; // Set MovementDirection based on RightDotProduct
		MovementDirection = 2.0f;
	}

	// Print the value of MovementDirection
	UE_LOG(LogTemp, Warning, TEXT("MovementDirection: %f"), MovementDirection);
}

void ULostAnimInstance::SetForwardDotProduct(float Value)
{
	ForwardDotProduct = Value;
}

void ULostAnimInstance::SetRightDotProduct(float Value)
{
	RightDotProduct = Value;
}