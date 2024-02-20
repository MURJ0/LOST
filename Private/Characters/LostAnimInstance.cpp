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

		UpdateCharacterDirection();
	}
}

void ULostAnimInstance::UpdateCharacterDirection()
{
    if (LostCharacterMovement) {
        // Get the velocity vector
        FVector Velocity = LostCharacterMovement->Velocity;

        // Project the velocity vector onto the horizontal plane (ignore the Z component)
        Velocity.Z = 0.0f;

        // Normalize the velocity vector
        if (!Velocity.IsNearlyZero()) {
            // Calculate the angle between the character's forward vector and its velocity vector
            float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(LostCharacter->GetActorForwardVector(), Velocity.GetSafeNormal())));

            // Get the cross product of the character's forward vector and the velocity vector to determine if the character is turning left or right
            FVector CrossProduct = FVector::CrossProduct(LostCharacter->GetActorForwardVector(), Velocity.GetSafeNormal());
            float Sign = FMath::Sign(CrossProduct.Z);

            // Map the angle to the range [-180, 180]
            float MappedAngle = Angle * Sign;

            // Convert the mapped angle to a value in the range [0, 1]
            float Direction = (MappedAngle + 180.0f) / 360.0f;

            // Set the CharacterDirection property
            CharacterDirection = FVector(Direction, 0.0f, 0.0f);
        }
    }
}