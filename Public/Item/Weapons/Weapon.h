// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Enemy/Enemy.h"
#include "Weapon.generated.h"

class USceneComponent;
class USoundWave;
class UBoxComponent;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	OneHanded,
	TwoHanded,
	Shield,
	NONE
};

UCLASS()
class LOSTV2_API AWeapon : public AItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Property")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Property")
	float Damage = 20.f;

	void BoxTrace(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere, Category = "Weapon Property")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Property")
	bool bShowBoxDebug = false;

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }

	TArray<AActor*> IgnoreActors;

	EWeaponType GetWeaponType() const { return WeaponType; }

	AEnemy* EnemyActor;
protected:

	UPROPERTY(EditAnywhere, Category = "Weapon Property")
	EWeaponType WeaponType;

	virtual void BeginPlay() override;

	// virtual added cuz argument list is too long and may not be seen !!!
	virtual void OnSpherOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	// virtual added cuz argument list is too long and may not be seen !!!
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ExecuteGetHit(FHitResult& BoxHit);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);
};
