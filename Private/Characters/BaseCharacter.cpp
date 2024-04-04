#include "Characters/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

ABaseCharacter::ABaseCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject< UAttributeComponent>(TEXT("Attributes"));
}


bool ABaseCharacter::AreAttributeNotNull() const 
{
	if (Attributes) {
		return true;
	}
	else {
		return false;
	}
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	// when the enemy gets hit will play the meta sound 
	PlayHitSound(ImpactPoint);
	
	// showing particles in the place where the enemy got hit
	PlayHitParticle(ImpactPoint);
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound) {
		if (HitSound) {
			UGameplayStatics::PlaySoundAtLocation(
				this,
				HitSound,
				ImpactPoint
			);
		}
	}
}

void ABaseCharacter::PlayHitParticle(const FVector& ImpactPoint)
{
	if (HitParticle && GetWorld()) {
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticle,
			ImpactPoint
		);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes) {
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if(CombatTarget == nullptr){
		return FVector();
	}

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;

	return CombatTargetLocation + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget) {
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::StopMontage(UAnimMontage* &Montage)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage) {
		AnimInstance->Montage_Stop(0.25f, Montage);
	}
}

void ABaseCharacter::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
}

void ABaseCharacter::DireactionalHitReact(const FVector& ImpactPoint)
{
	// Play the respective section of the montage
	//PlayGetHitMontage(SectionName);

	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// Forward * ToHit = magnitude of |Forward| and |ToHit| * cos(theta)
	// ==> |Forward| = 1 ; |ToHit| = 0 ; 
	// ==> Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);

	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);

	//convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// If CrossProduct points down, Theta shoud be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	if (CrossProduct.Z < 0) {
		Theta *= -1.f;
	}

	const float Threshold = 0.0f;

	// the enemy is taking hit from left
	if (Theta > 0.f && Theta <= 180.f) {
		PlayGetHitMontage(FName("Hit2"));
	}

	// the enemy is taking hit from right
	else if (Theta < 0.f && Theta >= -180.f) {
		PlayGetHitMontage(FName("Hit1"));
	}
}

void ABaseCharacter::PlayGetHitMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && GetHitMontage) {
		AnimInstance->Montage_Play(GetHitMontage);
		AnimInstance->Montage_JumpToSection(SectionName, GetHitMontage);
	}
}

