#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/PickUpInterface.h"
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
class ATreasure;

UCLASS()
class LOSTV2_API ALostV2Character : public ABaseCharacter, public IPickUpInterface
{
	GENERATED_BODY()

	//TODO: enhanced input that focus the enemy in the battle mode sphere with widget image (".") on the enemy. Echo must be facing only the enemy this enemy
	// Like souls like games
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

	virtual void SetOverlappingItem(class AItem* Item) override;
	virtual void AddSouls(class ASoul* Soul) override;
	virtual void AddGold(class ATreasure* Gold) override;

	void AddXP(float Value);

	//FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

	FORCEINLINE ELostDeathPose GetLostDeathPose() const { return LostDeathPose; }

	UFUNCTION(BlueprintCallable)
	EActionState GetActionStateInControlRig() const { return ActionState; }

	void SetHUDVisible();
	void SetHUDHidden();
	void SetCameraZoomToBattleMode();
	void SetCameraZoomToDefault();

	bool bIsCharacterInBattleMode = false;

	bool IsCameraRotatingWithCharacter();

	bool IsCharacterLockedOnTheClosestEnemy() const { return bIsCharacterLockedOnTheClosestEnemy; }
protected:
	TArray<AEnemy*> DetectedEnemies; // Declare an array to store detected enemies
	AEnemy* ClosestEnemy = nullptr;
	bool bIsCharacterLockedOnTheClosestEnemy = false;
	float RotationInterpSpeed = 10.0f;

	int32 AttackCounter = 0;

	int32 NumEnemiesInSphere = 0;

	FTimerHandle TimerHandle_ZoomOutAndHideHUD;
	float ZoomOutAndHUDHiddenDelay = 1.0f; // Adjust this value as needed

	void SetCameraZoomToDefaultAndHideHUDDelayed();

	bool bCanMove = true;

	// Attack method
	float LastSuccessfulAttackTime = 0.8f;
	float ComboResetTime = 2.0f; // Set the time before combo resets (in seconds)
	// Attack method
	
	virtual void BeginPlay() override;
	void Die();

	UPROPERTY(BlueprintReadOnly, Category = "DeathPose")
	ELostDeathPose LostDeathPose;

	UPROPERTY(BlueprintReadOnly, Category = "RestingPose")
	ELostRestingPose LostRestingPose;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* LostMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* HealAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ChangeCamera;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* HUDAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LockTargetAction;

	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void StartSprinting();
	void StopSprinting();
	void Attack();
	virtual void Jump() override;
	void EKeyPressed();
	void SetDodgeCostForDifferentTypeOfWeapon();
	void Dodge();
	void Heal();
	void ChangeCameraAngle();
	void SetHUDVisability();
	void LockTarget();

	// <CameraBoom arm lenght changes>
	float StartArmLength = 300.f;
	float TargetArmLength = 300.f;
	float DefaultArmLength = 200.f;
	bool bIsChangingArmLength = false;
	float CurrentArmLengthTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Settings")
	float CameraBoomSmoothTransition;

	int32 CountCanemraLenghtBoolsForZoom = 0;
	int32 CountCanemraLenghtBoolsForZoomOUT = 0;

	bool IsCharacterMoving(UCharacterMovementComponent* CharacterMovementSpeed);
	// </CameraBoom arm lenght changes>

	// <CameraBoom socketchanges>
	FVector StartSocketOffset;
	FVector TargetSocketOffset;
	float CurrentInterpolationTime;
	bool bIsChangingSocketOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Settings")
	float SocketOffsetInterpolationTime;
	// </CameraBoom socketchanges>

	bool CanChangeCameraBoomSocketOffset();
	bool CanChangeCameraBoomArmLenght();

	void ChangeCameraBoomArmLenght(float DeltaTime);
	void ChangeCameraBoomSocketOffset(float DeltaTime);

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WalkSpeed = 125.f;

	bool HasEnoughStamina();

	bool IsCharacterStateUnoccupied();
	bool IsActionStateUnoccupied();

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

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HealMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* FromIdleToWlakRunMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* ReachOutMontage;

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

	UFUNCTION(BlueprintCallable)
	void StartBonfireEffect();

	// healing when resting 
	UFUNCTION(BlueprintCallable)
	void StartHealing();

	UFUNCTION(BlueprintCallable)
	void StartResting();

	UFUNCTION(BlueprintCallable)
	void StopResting();

	// Rolling collision ON/OFF
	UFUNCTION(BlueprintCallable)
	void CharacterCollisionOnWhenDodging();

	UFUNCTION(BlueprintCallable)
	void CharacterCollisionOFFWhenDodging();

	bool bIsResting = false;

	// <BaseCharacter*>
	virtual void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled) override;
	virtual void DireactionalHitReact(const FVector& ImpactPoint) override;
	virtual void PlayGetHitMontage(const FName& SectionName) override;
	// </BaseCharacter*>
	
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Sphere;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	int EnemyInSpehere = 0;
	int EnemyInTheEndOfTheSphere = 0;

	class ULostAnimInstance* AnimInstanceForMovement;
private:
	UPROPERTY(EditAnywhere)
	double CombatRadius = 1000.f;

	void InitializeLostOverlay(APlayerController* PlayerController);
	void SetHealthHUD();
	
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

	// shift settings 
	bool bIsShiftPressed = false;
};
