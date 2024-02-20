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
	FVector CharacterDirection; // Character direction vector

public:
	void UpdateCharacterDirection();
};

