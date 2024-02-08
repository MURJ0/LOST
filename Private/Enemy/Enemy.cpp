#include "Enemy/Enemy.h"
#include "Item/Weapons/Weapon.h"
#include <Characters/LostV2Character.h>
#include "AIController.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/HealthBarComponent.h"

#include "Kismet/KismetSystemLibrary.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SetPeripheralVisionAngle(45.f);
	PawnSensing->SightRadius = 2000.f;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	HideHealthBar();

	EnemyController = Cast<AAIController>(GetController());

	MoveToTarget(PatrolTarget);

	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;

	Tags.Add(FName("Enemy"));

	if (PawnSensing) {
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	// equiping the weapons in the right and left hand to their correct sockets
	AEnemy::EquipWeaponsAtBeginPlay();

}

void AEnemy::EquipWeaponsAtBeginPlay()
{
	// equipping every type of weapon to his correct socket
	UWorld* World = GetWorld();

	// for the right hand --> in the right hand can be only two types of weapons --> Twohanded weapon( if the weapon is twohanded the enemy cant have anything else in the left hand ) and onehanded weapon
	if (World && RightHandWeaponClass) {
		RightHandEquippedWeapon = World->SpawnActor<AWeapon>(RightHandWeaponClass);

		if (IsRightHandWeaponTwoHanded()) { 
			RightHandEquippedWeapon->Equip(GetMesh(), FName("SpineSocket"), this, this); 
		}

		if (IsRightHandWeaponOneHanded()) { 
			RightHandEquippedWeapon->Equip(GetMesh(), FName("HipsSocket"), this, this); 
		}
		
		RightHandEquippedWeapon->DeactivateEmbersEffect();
	}

	// for the left hand --> in the left hand must be only to types of weapons - shield and onehanded weapon!!
	if (World && LeftHandWeaponClass) {
		LeftHandEquippedWeapon = World->SpawnActor<AWeapon>(LeftHandWeaponClass);

		if (IsLeftHandWeaponOneHanded()) {
			LeftHandEquippedWeapon->Equip(GetMesh(), FName("HipsSocket"), this, this);
		}
		
		if (IsLeftHandWeaponShield()) {
			LeftHandEquippedWeapon->Equip(GetMesh(), FName("HipsSocket"), this, this);
		}
		LeftHandEquippedWeapon->DeactivateEmbersEffect();
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (DeathPose != EDeathPose::EDP_Alive) { return; }

	if (EnemyState > EEnemyState::EES_Patrolling) {
		CheckCombatTarget();
	}
	else {
		ChackPatrolTarget();
	}

	if (EnemyState == EEnemyState::EES_Chasing) {
		if (!bArmed) {
			PlayArmDisarmMontage(TEXT("Arm"));
			bArmed = true;
			bDisarmed = false;
		}
	}
}

void AEnemy::ChackPatrolTarget()
{
	// random number that is made in timer so the enemy stays in one position for (random time) when it reache the patrol target
	const float WaitTime = FMath::FRandRange(PatrolWaitMin, PatrolWaitMax); 

	if (InTargetRange(PatrolTarget, PatrolRadius)) {
		PatrolTarget = ChoosePatrolTarget();
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimeFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius()) {
		ClearAttackTimer();
		LoseInterest();
		StartPatrolling();
	}
	else if (IsOutsideAttackRadius() && EnemyState != EEnemyState::EES_Chasing) {
		ClearAttackTimer();
		StartChasing();
	}
	else if (CanAttack()) {
		StartAttackTimer();
	}
}

bool AEnemy::CanAttack()
{
	return IsInsideAttackRadius() && EnemyState != EEnemyState::EES_Attacking && DeathPose == EDeathPose::EDP_Alive ;
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);

	if (!bDisarmed) {
		PlayArmDisarmMontage(TEXT("Disarm"));
		bArmed = false;
		bDisarmed = true;
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false);
	}
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius) ;
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

void AEnemy::StartChasing()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

void AEnemy::Attack()
{
	//if (!bIsAttacking) {
	//	return; // Skip attack() if the enemy got hit 
	//}

	// if Echo is dead enemy will stop attacking 
	if (CombatTarget->ActorHasTag(TEXT("Dead"))) {
		CombatTarget = nullptr;
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsRightHandWeaponOneHanded() && !LeftHandEquippedWeapon) {
		PlayMontage(AnimInstance, AttackMontage, TEXT("OneHandCombo"));
		ClearAttackTimer();
		return;
	}

	if (IsRightHandWeaponTwoHanded() && !LeftHandEquippedWeapon) {
		PlayMontage(AnimInstance, AttackMontage, TEXT("TwoHand2"));
		ClearAttackTimer();
		return;
	}

	if (IsRightHandWeaponOneHanded() && IsLeftHandWeaponShield()) {
		PlayMontage(AnimInstance, AttackMontage, TEXT("SwordAndShield"));
		ClearAttackTimer();
		return;
	}
	
	if (IsRightHandWeaponOneHanded() && IsLeftHandWeaponOneHanded()) {
		PlayMontage(AnimInstance, AttackMontage, TEXT("DualWeapon"));
		ClearAttackTimer();
		return;
	}
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	bIsAttacking = true;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	ShowHealthBar();

	//bIsAttacking = false;

	ClearPatrolTimer();
	ClearAttackTimer();

	DisableWeaponBoxCollision(RightHandEquippedWeapon);
	DisableWeaponBoxCollision(LeftHandEquippedWeapon);

	StopAttackMontage();

	// when the enemy gets hit will play the meta sound 
	PlayHitSound(ImpactPoint);

	// showing particles in the place where the enemy got hit
	PlayHitParticle(ImpactPoint);

	if (!bArmed) {
		PlayArmDisarmMontage(TEXT("Arm"));
		bArmed = true;
		bDisarmed = false;
		bIsGettingHit = false;
		StartAttackTimer();
		return;
	}

	if (IsAlive()) {
		DireactionalHitReact(ImpactPoint);
		StartAttackTimer();
	}
	else {
		Die();
	}
}

void AEnemy::DireactionalHitReact(const FVector& ImpactPoint)
{
	Super::DireactionalHitReact(ImpactPoint);
	//bIsGettingHit = false;
}

void AEnemy::PlayGetHitMontage(const FName& SectionName)
{
	Super::PlayGetHitMontage(SectionName);
}

void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	HideHealthBar();

	if (AnimInstance && DeathMontage) {
		AnimInstance->Montage_Play(DeathMontage);
		const int32 Selection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch(Selection) {
		case 0: SectionName = FName("Death1");
			DeathPose = EDeathPose::EDP_Death1; 
			break;
		case 1: SectionName = FName("Death2");
			DeathPose = EDeathPose::EDP_Death2; 
			break;
		default: break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
	
	DestroyWeapons(); // Destroy every equiped weapon when the enemy dies 
	
	DisableCapsule(); // Setting the collision of the capsule to IGNORE PAWN after the enemy dies so the character can go through  

	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore); // Setting the collision for the mesh to ignore to prevent any collision after the enemy is dead

	SetLifeSpan(3.f); // Destroy the instance of the BP_Enemy  
}

void AEnemy::DestroyWeapons()
{
	if (RightHandEquippedWeapon) {
		RightHandEquippedWeapon->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RightHandEquippedWeapon->SetLifeSpan(3.f);
		RightHandEquippedWeapon = nullptr;
	}

	if (LeftHandEquippedWeapon) {
		LeftHandEquippedWeapon->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		LeftHandEquippedWeapon->SetLifeSpan(3.f);
		LeftHandEquippedWeapon = nullptr;
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(15.f);
	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*>ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}
	const int32 NumPatrolTargets = ValidTargets.Num();

	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		DeathPose == EDeathPose::EDP_Alive &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState != EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));
	
	// if the enemy is alive, its not already chasing or attacking and the pawn he is detecting the playable character
	if (bShouldChaseTarget) {
		CombatTarget = SeenPawn;
		if (CombatTarget->ActorHasTag(TEXT("Dead"))) {
			CombatTarget = nullptr;
			return;
		}
		ClearPatrolTimer(); // If it reaches the target, clear the timer that's causing the enemy to stay in one position.
		StartChasing();
	}
}

void AEnemy::PatrolTimeFinished()
{
	MoveToTarget(PatrolTarget);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	
	if (IsInsideAttackRadius()) {
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius()) {
		StartChasing();
	}

	return DamageAmount;
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget) {
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::PlayArmDisarmMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (RightHandEquippedWeapon) {
		if (IsRightHandWeaponOneHanded() && !LeftHandEquippedWeapon) {
			PlayMontage(AnimInstance, OnehandedEquipUnequipMontage, SectionName);
			return;
		}

		if (IsRightHandWeaponTwoHanded() && !LeftHandEquippedWeapon) {
			PlayMontage(AnimInstance, TwoHandedWeaponEquipUnequipMontage, SectionName);
			return;
		}

		if (IsRightHandWeaponOneHanded() && IsLeftHandWeaponShield()) {
			PlayMontage(AnimInstance, SwordAndShieldEquipUnequipMontage, SectionName);
			return;
		}

		if (IsRightHandWeaponOneHanded() && IsLeftHandWeaponOneHanded()) {
			PlayMontage(AnimInstance, OnehandedEquipUnequipMontage, SectionName);
			return;
		}
	}
}

bool AEnemy::IsRightHandWeaponTwoHanded()
{
	return RightHandEquippedWeapon->GetWeaponType() == EWeaponType::TwoHanded;
}

void AEnemy::DisableWeaponBoxCollision(AWeapon* EquippedWeapon)
{	
	if (EquippedWeapon) {
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

bool AEnemy::IsRightHandWeaponOneHanded()
{
	return RightHandEquippedWeapon->GetWeaponType() == EWeaponType::OneHanded;
}

bool AEnemy::IsLeftHandWeaponShield()
{
	return LeftHandEquippedWeapon->GetWeaponType() == EWeaponType::Shield;
}

bool AEnemy::IsLeftHandWeaponOneHanded()
{
	return LeftHandEquippedWeapon->GetWeaponType() == EWeaponType::OneHanded;
}

void AEnemy::PlayMontage(UAnimInstance*& AnimInstance, UAnimMontage*& WeaponMontage, FName SectionName)
{
	if (AnimInstance && WeaponMontage) {
		AnimInstance->Montage_Play(WeaponMontage);
		AnimInstance->Montage_JumpToSection(SectionName, WeaponMontage);
	}
}

void AEnemy::Equip()
{
	if (RightHandEquippedWeapon) {
		RightHandEquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}

	if (LeftHandEquippedWeapon) {
		LeftHandEquippedWeapon->AttachMeshToSocket(GetMesh(), FName("LeftHandSocket"));
	}

}

void AEnemy::Unequip()
{
	if (RightHandEquippedWeapon)
	{
		if (IsRightHandWeaponTwoHanded())
		{
			RightHandEquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
			return;
		}

		if (IsRightHandWeaponOneHanded())
		{
			RightHandEquippedWeapon->AttachMeshToSocket(GetMesh(), FName("HipsSocket"));
		}
	}

	if (LeftHandEquippedWeapon)
	{
		if (IsLeftHandWeaponOneHanded())
		{
			LeftHandEquippedWeapon->AttachMeshToSocket(GetMesh(), FName("HipsSocket"));
			return;
		}

		if (IsLeftHandWeaponShield()) {
			LeftHandEquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineShieldSocket"));
			return;
		}
	}
	
}

void AEnemy::CanMove()
{
	EnemyState = EEnemyState::EES_NoState;
	ChackPatrolTarget();
}

void AEnemy::CannotMove()
{
	GetCharacterMovement()->MaxWalkSpeed = StopSpeed;
}

void AEnemy::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (RightHandEquippedWeapon && RightHandEquippedWeapon->GetWeaponBox()) {
		RightHandEquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		RightHandEquippedWeapon->IgnoreActors.Empty();
	}
	if (LeftHandEquippedWeapon && LeftHandEquippedWeapon->GetWeaponBox()) {
		LeftHandEquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		LeftHandEquippedWeapon->IgnoreActors.Empty();
	}
}

//if (RightHandEquippedWeapon && RightHandEquippedWeapon->GetWeaponBox()) {
//	UPrimitiveComponent* WeaponBox = RightHandEquippedWeapon->GetWeaponBox();
//	if (WeaponBox) {
//		WeaponBox->SetCollisionEnabled(CollisionEnabled);
//	}
//	RightHandEquippedWeapon->IgnoreActors.Empty();
//}
//
//if (LeftHandEquippedWeapon && LeftHandEquippedWeapon->GetWeaponBox()) {
//	UPrimitiveComponent* WeaponBox = LeftHandEquippedWeapon->GetWeaponBox();
//	if (WeaponBox) {
//		WeaponBox->SetCollisionEnabled(CollisionEnabled);
//	}
//	LeftHandEquippedWeapon->IgnoreActors.Empty();
//}