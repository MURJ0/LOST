#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Class.h"
#include "Item/Treasure/Treasure.h"
#include "Components/CapsuleComponent.h"
#include "Characters/LostV2Character.h"



ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (GeometryCollection)
	{
		GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnBreakEvent);
	}
}

void ABreakableActor::OnBreakEvent(const FChaosBreakEvent& BreakEvent)
{
	//if (!bBroken)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Breaking the pod"));
	//	UWorld* World = GetWorld();
	//	if (World && TreasureClasses.Num() > 0)
	//	{
	//		bBroken = true;
	//
	//		FVector Location = GetActorLocation();
	//		Location.Z += 75.f;
	//
	//		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);
	//
	//		// Spawn the treasure
	//		World->SpawnActor<ATreasure>(TreasureClasses[Selection], Location, GetActorRotation());
	//		// Implement any logic related to the spawned treasure here, if needed
	//		if (BreakSound) {
	//			UGameplayStatics::PlaySoundAtLocation(
	//				this,
	//				BreakSound,
	//				GetActorLocation()
	//			);
	//		}
	//	}
	//}
}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (!bBroken) {
		UWorld* World = GetWorld();
		if (World && TreasureClasses.Num() > 0)
		{
			bBroken = true;

			FVector Location = GetActorLocation();
			Location.Z += 75.f;

			const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);

			// Spawn the treasure
			World->SpawnActor<ATreasure>(TreasureClasses[Selection],Location,  GetActorRotation());
			// Implement any logic related to the spawned treasure here, if needed
			if (BreakSound) {
				UGameplayStatics::PlaySoundAtLocation(
					this,
					BreakSound,
					GetActorLocation()
				);
			}
		}
		
	}
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


