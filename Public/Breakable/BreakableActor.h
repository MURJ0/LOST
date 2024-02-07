#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "Chaos/ChaosEngineInterface.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"
#include "BreakableActor.generated.h"


UCLASS()
class LOSTV2_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	TArray<class TSubclassOf<class ATreasure>> TreasureClasses;

	UPROPERTY(EditAnywhere, Category = "BreakSound")
	class USoundBase* BreakSound;

	bool bBroken = false;
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBreakEvent(const FChaosBreakEvent& BreakEvent);

	UPROPERTY(VisibleAnywhere)
	class UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;

	
};
