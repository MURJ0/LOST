#include "Item/Weapons/Weapon.h"
#include "Characters/LostV2Character.h"
#include "Enemy/Enemy.h"
#include "Sound/SoundWave.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "HUD/InteractComponent.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"


AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::Tick(float DeltaTime)
{
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);

	if (InteractWidget) {
		InteractWidget->SetInteractText(TEXT("Pick up"));
		InteractWidget->HideInteractText();
	}
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	if (InteractWidget) {
		InteractWidget->HideInteractText();
	}
	bIsEquipped = true;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}

	AttachMeshToSocket(InParent, InSocketName);
	ItemState = EItemState::EIS_Equipped;

	DeactivateEmbersEffect();
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::OnSpherOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSpherOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ALostV2Character* Character = Cast<ALostV2Character>(OtherActor);
	if (Character && InteractWidget && !bIsEquipped) {
		InteractWidget->ShowInteractText();
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) 
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	ALostV2Character* Character = Cast<ALostV2Character>(OtherActor);
	if (Character && InteractWidget) {
		InteractWidget->HideInteractText();
	}
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
	{
		return; // Skip processing if the overlapped actor is the owner of the weapon
	}

	if (GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"))) {
		return;
	}

	FHitResult BoxHit; 
	BoxTrace(BoxHit);

	if (BoxHit.GetActor()) {
		if (GetOwner()->ActorHasTag(TEXT("Enemy")) && BoxHit.GetActor()->ActorHasTag(TEXT("Enemy"))) {
			return;
		}
		ALostV2Character* Character = Cast<ALostV2Character>(GetOwner());
		AEnemy* Enemy = Cast<AEnemy>(GetOwner());
		if (Character && Character->AreAttributeNotNull()) {
			float DamageToApply = FMath::RandRange(Character->GetAttribute()->GetMinDamage(), Character->GetAttribute()->GetMaxDamage());
			UGameplayStatics::ApplyDamage(BoxHit.GetActor(), DamageToApply, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		}
		else if(Enemy && Enemy->AreAttributeNotNull()){
			float DamageToApply = FMath::RandRange(Enemy->GetAttribute()->GetMinDamage(), Enemy->GetAttribute()->GetMaxDamage());
			UGameplayStatics::ApplyDamage(BoxHit.GetActor(), DamageToApply, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		}
		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
	}
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast <IHitInterface>(BoxHit.GetActor());
	if (HitInterface) {
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint);
	}
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	for (AActor* Actor : IgnoreActors) {
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End, 
		BoxTraceExtent,
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit,
		true
	);
	IgnoreActors.AddUnique(BoxHit.GetActor());
}

