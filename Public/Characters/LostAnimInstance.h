// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "LostAnimInstance.generated.h"

UCLASS()
class LOSTV2_API ULostAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	class ALostV2Character* LostCharacter;

	UPROPERTY(BlueprintReadOnly = Category = Movement)
	class UCharacterMovementComponent* LostCharacterMovement;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement | Character State")
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = State)
	EActionState ActionState;
	
	UPROPERTY(BlueprintReadOnly, Category = State)
	ELostDeathPose LostDeathPose;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float ForwardDotProduct; // Forward dot product value

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float RightDotProduct; // Right dot product value

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float MovementDirection; // Movement direction (0: forward, 1: right, 2: backward, 3: left)

public:
	void SetForwardDotProduct(float Value);
	void SetRightDotProduct(float Value);
	void UpdateCharacterDirection();
};

