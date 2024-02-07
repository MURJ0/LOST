#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"


UCLASS()
class LOSTV2_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()
public:
	AEnemy();

	// <AActor>
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	// </AActor>

	// <UHitInterface> 
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	// </UHitInterface> 

protected:
	virtual void BeginPlay() override;
	
	void Die();
	void DestroyWeapons();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	void Attack();
	AActor* ChoosePatrolTarget();
	virtual void HandleDamage(float DamageAmount) override;

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	UFUNCTION(BlueprintCallable, Category = Equipment)
	void Equip();

	UFUNCTION(BlueprintCallable, Category = Equipment)
	void Unequip();

	UFUNCTION(BlueprintCallable)
	void CanMove();

	UFUNCTION(BlueprintCallable)
	void CannotMove();

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	virtual void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled) override;

private:
	// Navigation
	UPROPERTY()
	class AAIController* EnemyController;

	//CombatTarget and radiuses

	UPROPERTY(EditAnywhere)
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 500.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	//Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	// Components
	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* PawnSensing;

	UPROPERTY(VisibleAnywhere, Category = HealthBar)
	class UHealthBarComponent* HealthBarWidget;

	// Equip and Arm/Disarm methods
	void EquipWeaponsAtBeginPlay();

	void PlayArmDisarmMontage(FName SectionName);
	void PlayMontage(UAnimInstance*& AnimInstance, UAnimMontage*& TwoHandedWeaponMontage, FName SectionName);

	// Arm/Disarm Montange
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* TwoHandedWeaponEquipUnequipMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* OnehandedEquipUnequipMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* SwordAndShieldEquipUnequipMontage;

	// <*BaseCharacter>
	virtual void DireactionalHitReact(const FVector& ImpactPoint) override;
	virtual void PlayGetHitMontage(const FName& SectionName) override;
	// </*BaseCharacter>
	
	// Enemy States
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
	EEnemyStateAtThatMoment EnemyStateAtThisMoment = EEnemyStateAtThatMoment::EES_Unoccupied;

	// Weapons
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> RightHandWeaponClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> LeftHandWeaponClass;
	
	class AWeapon* RightHandEquippedWeapon;
	class AWeapon* LeftHandEquippedWeapon;

	bool bDisarmed = true;
	bool bArmed = false;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* TwoHandMontage;

	// Weapon types 
	void DisableWeaponBoxCollision(AWeapon* EquippedWeapon);

	bool IsRightHandWeaponOneHanded();
	bool IsRightHandWeaponTwoHanded();

	bool IsLeftHandWeaponShield();
	bool IsLeftHandWeaponOneHanded();

	// AI behavior
	void ChackPatrolTarget();
	void CheckCombatTarget();

	void LoseInterest();
	void StartPatrolling();
	void StartChasing();

	void ShowHealthBar();
	void HideHealthBar();

	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool CanAttack();

	// Combat
	bool bIsGettingHit;

	// EnemySpeed
	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 120.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float StopSpeed = 0.f;

	// Attack timers
	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;

	void StartAttackTimer();
	void ClearAttackTimer();
	void ClearPatrolTimer();

	// Patrol timer
	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;

	void PatrolTimeFinished();

	bool bIsAttacking;
};
