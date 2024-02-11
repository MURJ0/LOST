// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UStaticMeshComponent;
class UNiagaraComponent;
class USphereComponent;
class UNiagaraSystem;
class SoundBase;

enum class EItemState : uint8 {
	EIS_Hovering,
	EIS_Equipped
};


UCLASS()
class LOSTV2_API AItem : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AItem();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnSpherOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DeactivateEmbersEffect();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SpawnPickUpSystem();
	virtual void SpawnPickUpSound();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(EditAnywhere, Category = NiagaraComponent)
	UNiagaraComponent* ItemEffect;

	UPROPERTY(EditAnywhere)
	USoundBase* PickUpSound;
private:
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* PickUpEffect;

};
