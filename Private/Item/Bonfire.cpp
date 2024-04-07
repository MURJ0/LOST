// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Bonfire.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/LostV2Character.h"

ABonfire::ABonfire()
{
	ItemMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void ABonfire::Tick(float DeltaTime)
{

}

void ABonfire::OnSpherOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSpherOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ABonfire::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void ABonfire::ActivateBonfire()
{
	bActive = true;
	if (ItemEffect) {
		ItemEffect->Activate();
	}
}

void ABonfire::BeginPlay()
{
	Super::BeginPlay();

	BonfireLocation = GetActorLocation();
	
	ItemEffect->Deactivate();
}
