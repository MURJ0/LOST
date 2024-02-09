#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "LostV2Character.generated.h"

class USkeletalMeshComponent;
class UInputAction;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;
class AWeapon;
class ULostOverlay;

UCLASS()
class LOSTV2_API ALostV2Character : public ABaseCharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY()
	ULostOverlay* LostOverlay;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

public:
	ALostV2Character();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//  <BaseCharacter>
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	//  </BaseCharacter>

	// <ABaseCharacter> 
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	// </ABaseCharacter> 

	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

	FORCEINLINE ELostDeathPose GetLostDeathPose() const { return LostDeathPose; }

	UFUNCTION(BlueprintCallable)
	EActionState GetActionStateInControlRig() const { return ActionState; }
protected:
	int32 AttackCounter = 0;

	bool bCanMove = true;

	virtual void BeginPlay() override;
	void Die();

	UPROPERTY(BlueprintReadOnly, Category = "DeathPose")
	ELostDeathPose LostDeathPose;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* LostMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AttackAction;

	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void EKeyPressed();
	bool IsCharacterStateUnoccupied();
	bool IsActionStateUnoccupied();
	void Attack();
	virtual void Jump() override;

	/*
	Animation montages
	*/
	void PlayMontage(UAnimMontage* Montage, FName SectionName);

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* OneHandedAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EmptyHandedAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipUnequipMontage;

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void EquipEnd();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void CanMove();

	UFUNCTION(BlueprintCallable)
	void CannotMove();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	// <BaseCharacter*>
	virtual void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled) override;
	virtual void DireactionalHitReact(const FVector& ImpactPoint) override;
	virtual void PlayGetHitMontage(const FName& SectionName) override;
	// </BaseCharacter*>

private:
	void InitializeLostOverlay(APlayerController* PlayerController);
	void SetHealthHUD();
};
