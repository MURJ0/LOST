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
#include "Item/Soul.h"
#include "Item/Treasure/Treasure.h"
#include "Animation/AnimMontage.h"
#include "Enemy/Enemy.h"
#include "HUD/LostHUD.h"
#include "HUD/LostOverlay.h"

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
		InitializeLostOverlay(PlayerController);
	}

	//making tag for the LostCharacter so I can use it in the PawnSeen method in enemy.cpp
	Tags.Add(FName("EngageableTarget"));
}

void ALostV2Character::InitializeLostOverlay(APlayerController* PlayerController)
{
	ALostHUD* LostHUD = Cast<ALostHUD>(PlayerController->GetHUD());
	if (LostHUD) {
		LostOverlay = LostHUD->GetLostOverlay();
		if (LostOverlay) {
			LostOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
			LostOverlay->SetStaminaBarPercent(1.f);
			LostOverlay->SetGold(0);
			LostOverlay->SetGold(0);
		}
	}
}

void ALostV2Character::Move(const FInputActionValue& Value)
{
	if (!IsActionStateUnoccupied() && !bCanMove) {
		return;
	}

	if (ActionState == EActionState::EAS_Dead) return;
	else {
		const FVector2D MovementVector = Value.Get< FVector2D>();
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
		SetDodgeCostForDifferentTypeOfWeapon();
	}
	else if (EquippedWeapon){
		if (IsActionStateUnoccupied() && !IsCharacterStateUnoccupied()) {
			PlayMontage(EquipUnequipMontage, FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
		}
		else if (IsActionStateUnoccupied() && IsCharacterStateUnoccupied() && EquippedWeapon != nullptr) {
			PlayMontage(EquipUnequipMontage, FName("Equip"));
			CharacterState = ECharacterState::ECS_EquipedOneHandedWeapon;
		}
		ActionState = EActionState::EAS_Unoccupied;
	}
}

void ALostV2Character::SetDodgeCostForDifferentTypeOfWeapon()
{
	if (EquippedWeapon) {
		if (EquippedWeapon->GetWeaponType() == EWeaponType::OneHanded) {
			Attributes->SetDodgeCost(25.f);
		}

		if (EquippedWeapon->GetWeaponType() == EWeaponType::TwoHanded) {
			Attributes->SetDodgeCost(35.f);
		}
	}
}

bool ALostV2Character::IsCharacterStateUnoccupied()
{
	return CharacterState == ECharacterState::ECS_Unequipped;
}

bool ALostV2Character::IsActionStateUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ALostV2Character::Attack()
{
	if (IsActionStateUnoccupied()) {
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

void ALostV2Character::Jump()
{
	if (IsActionStateUnoccupied()) {
		Super::Jump();
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

	if (Attributes && LostOverlay) {
		Attributes->RegenStamina(DeltaTime);
		LostOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
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

		//Dodge
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ALostV2Character::Dodge);

		//Heal
		EnhancedInputComponent->BindAction(HealAction, ETriggerEvent::Triggered, this, &ALostV2Character::Heal);
	}
}

void ALostV2Character::Dodge()
{
	if (!IsActionStateUnoccupied() || !HasEnoughStamina()) { return; }

	if (ActionState == EActionState::EAS_Dodging) {	return;	}

	ActionState = EActionState::EAS_Dodging;
	PlayMontage(DodgeMontage, FName("Dodge")); // Plays DodgeMontage

	if (Attributes && LostOverlay) {
		Attributes->UseStamina(Attributes->GetDodgeCost());
		LostOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ALostV2Character::Heal()
{
	// TODO :: ABP implementation of healing
	if (HealMontage && IsActionStateUnoccupied()) {
		if (Attributes && LostOverlay) {
			Attributes->Heal();
			LostOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
		}
		UE_LOG(LogTemp, Warning, TEXT("Heal"));
		ActionState = EActionState::EAS_Attacking;
		PlayMontage(HealMontage, FName("Sitting"));
	}
}

bool ALostV2Character::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();;
}

float ALostV2Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHealthHUD();
	return DamageAmount;
}

void ALostV2Character::SetHealthHUD()
{
	if (LostOverlay && Attributes) {
		LostOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void ALostV2Character::GetHit_Implementation(const FVector& ImpactPoint)
{
	Super::GetHit_Implementation(ImpactPoint);
	
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

void ALostV2Character::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ALostV2Character::AddSouls(ASoul* Soul)
{
	if (Attributes && LostOverlay) {
		Attributes->AddSouls(Soul->GetSouls());
		LostOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ALostV2Character::AddGold(ATreasure* Gold)
{
	if (Attributes && LostOverlay) {
		Attributes->AddGold(Gold->GetGold());
		LostOverlay->SetGold(Attributes->GetGold());
	}
}

void ALostV2Character::Die()
{
	ActionState = EActionState::EAS_Dead;
	LostDeathPose = ELostDeathPose::ELDP_Death1;

	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);

	Tags.Add(FName("Dead"));

	PlayMontage(DeathMontage, FName("Death1"));
	
}

void ALostV2Character::PlayMontage(UAnimMontage *Montage, FName SectionName)
{
	UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
	
	if (AnimInstance && Montage) {
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
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
	ActionState = EActionState::EAS_Unoccupied;
	bCanMove = true;
}

void ALostV2Character::CannotMove()
{
	ActionState = EActionState::EAS_Attacking;
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
