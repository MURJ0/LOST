#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"

#include "BaseCharacter.generated.h"

UCLASS()
class LOSTV2_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	bool IsAlive();

	void PlayHitSound(const FVector& ImpactPoint);
	void PlayHitParticle(const FVector& ImpactPoint);

	virtual void HandleDamage(float DamageAmount);

	void DisableCapsule();

	// Animation Montages

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* GetHitMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	void StopAttackMontage();

	// Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	// BlueprintCallable 
	UFUNCTION(BlueprintCallable)
   	virtual void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

	// GettingHit
	virtual void DireactionalHitReact(const FVector& ImpactPoint);
	virtual void PlayGetHitMontage(const FName& SectionName);

	UPROPERTY(BlueprintReadOnly, Category = Enemy)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTargetDistance = 75.f;

private:
	// Sounds and Particles

	UPROPERTY(EditAnywhere, Category = HitSound)
	class USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = Effect)
	class UParticleSystem* HitParticle;

};
