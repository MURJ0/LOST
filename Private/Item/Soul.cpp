#include "Item/Soul.h"
#include "Interfaces/PickUpInterface.h"

void ASoul::OnSpherOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickUpInterface* PickUpInterface = Cast<IPickUpInterface>(OtherActor);
	if (PickUpInterface) {
		PickUpInterface->AddSouls(this);
		SpawnPickUpSystem();
		SpawnPickUpSound();
		Destroy();
	}
}
