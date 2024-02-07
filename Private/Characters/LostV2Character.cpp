#include "Characters/LostV2Character.h"

#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"

#include "GroomComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Item/Weapons/Weapon.h"
#include "Item/Item.h"
#include "Animation/AnimMontage.h"
#include <Enemy/Enemy.h>

ALostV2Character::ALostV2Character()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;  // Character moves in the direction of input ... 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f); // at this rotation rate

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // Setting the object type to WorldDynamic

	// Setting all channels to ignore exept Visibility and WorldDynamic
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject <USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoom);
	CameraComponent->bUsePawnControlRotation = false;
}

void ALostV2Character::BeginPlay()
{ 
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController) {
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem) {
			Subsystem->AddMappingContext(LostMappingContext, 0);
		}
	}
	
	//making tag for the LostCharacter so I can use it in the PawnSeen method in enemy.cpp
	Tags.Add(FName("EngageableTarget"));
}

void ALostV2Character::Move(const FInputActionValue& Value)
{
	if (ActionState == EActionState::EAS_Unoccupied && bCanMove) {
		const FVector2D MovementVector = Value.Get< FVector2D>();
			//UE_LOG(LogTemp, Warning, TEXT("Adding movement input."));
			if (Controller) {

				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				AddMovementInput(ForwardDirection, MovementVector.Y);

				const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				AddMovementInput(RightDirection, MovementVector.X);
			}
	}
}

void ALostV2Character::EKeyPressed()
{ 
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon) {
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this , this);
		
		CharacterState = ECharacterState::ECS_EquipedOneHandedWeapon;
		OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;
	}
	else if (EquippedWeapon){
		if (ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped) {
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
		}
		else if (ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon != nullptr) {
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquipedOneHandedWeapon;
		}
		ActionState = EActionState::EAS_Unoccupied;
	}
}

void ALostV2Character::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipUnequipMontage) {
		AnimInstance->Montage_Play(EquipUnequipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipUnequipMontage);
	}
}

float LastSuccessfulAttackTime = 0.8f;
float ComboResetTime = 2.0f; // Set the time before combo resets (in seconds)

void ALostV2Character::Attack()
{
	if (ActionState == EActionState::EAS_Unoccupied) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && OneHandedAttackMontage) {
			if (GetCharacterState() == ECharacterState::ECS_EquipedOneHandedWeapon) {
				bCanMove = false;
				float CurrentTime = GetWorld()->GetTimeSeconds();
				if (CurrentTime - LastSuccessfulAttackTime >= ComboResetTime) {
					AttackCounter = 1; // Reset combo if enough time has passed
				}
				else {
					if ((AttackCounter + 1) > 3) {
						AttackCounter = 1;
					}
					else {
						AttackCounter++;
					}
				}

				FString SectionName = FString::Printf(TEXT("Attack%d"), FMath::Clamp(AttackCounter, 1, 3));
				FName Selection = FName(*SectionName);

				AnimInstance->Montage_Play(OneHandedAttackMontage, 1.0f);
				AnimInstance->Montage_JumpToSection(Selection, OneHandedAttackMontage);
				LastSuccessfulAttackTime = CurrentTime;
				ActionState = EActionState::EAS_Attacking;
			}
		}
	}
}

void ALostV2Character::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}

}

void ALostV2Character::Tick(float DeltaTime)
{ 
	Super::Tick(DeltaTime);
}

void ALostV2Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent) {
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALostV2Character::Move);

		//Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALostV2Character::Look);

		//Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);

		//Equip
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ALostV2Character::EKeyPressed);

		//Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ALostV2Character::Attack);

	}
}

float ALostV2Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	return DamageAmount;
}

void ALostV2Character::GetHit_Implementation(const FVector& ImpactPoint)
{
	// when the enemy gets hit will play the meta sound 
	PlayHitSound(ImpactPoint);
	
	// showing particles in the place where the enemy got hit
	PlayHitParticle(ImpactPoint);


	if (IsAlive()) { // Check if Hitter is valid and not the player character itself
		DireactionalHitReact(ImpactPoint);
		ActionState = EActionState::EAS_HitReaction;
	}
	else {
		Die();
	}

	if (EquippedWeapon) {
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ALostV2Character::Die()
{
	ActionState = EActionState::EAS_Dead;
	LostDeathPose = ELostDeathPose::ELDP_Death1;

	UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();

	Tags.Add(FName("Dead"));

	if (AnimInstance && DeathMontage) {
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(TEXT("Death1"), DeathMontage);
	}
}

void ALostV2Character::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ALostV2Character::EquipEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ALostV2Character::Disarm()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ALostV2Character::Arm()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ALostV2Character::CanMove()
{
	bCanMove = true;
}

void ALostV2Character::CannotMove()
{
	bCanMove = false;
}

void ALostV2Character::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	CanMove();
}

void ALostV2Character::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox()) {
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ALostV2Character::DireactionalHitReact(const FVector& ImpactPoint)
{
	Super::DireactionalHitReact(ImpactPoint);
}

void ALostV2Character::PlayGetHitMontage(const FName& SectionName)
{
	Super::PlayGetHitMontage(SectionName);
}
