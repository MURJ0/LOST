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
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Item/Bonfire.h"
#include "HUD/InteractComponent.h"

#include "Item/Weapons/Weapon.h"
#include "Item/Item.h"
#include "Item/Soul.h"
#include "Item/Treasure/Treasure.h"
#include "Animation/AnimMontage.h"
#include "Enemy/Enemy.h"
#include "HUD/LostHUD.h"
#include "HUD/LostOverlay.h"
#include <Kismet/GameplayStatics.h>

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
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 25.f);
	CameraBoom->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoom);
	CameraComponent->bUsePawnControlRotation = false;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->InitSphereRadius(1000.0f); // Adjust the radius as needed
	Sphere->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // Set the collision profile to "OverlapAllDynamic"
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Set collision to query-only
	Sphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // Set collision object type to world dynamic (actors)
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // Ignore all collision channels
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap); // Allow overlap with actors
	Sphere->SetupAttachment(RootComponent);
}

void ALostV2Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes && LostOverlay) {
		Attributes->RegenStamina(DeltaTime);
		LostOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}

	if (bIsResting) {
		if (ActionState == EActionState::EAS_HitReaction) {
			bIsResting = false;
			StopMontage(HealMontage);
			return;
		}
		Attributes->RegenHealth(DeltaTime);
		LostOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}

	if (CanChangeCameraBoomArmLenght())
	{
		ChangeCameraBoomArmLenght(DeltaTime);
	}

	if (CanChangeCameraBoomSocketOffset())
	{
		ChangeCameraBoomSocketOffset(DeltaTime);
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

		//Sprint 
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ALostV2Character::StartSprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ALostV2Character::StopSprinting);

		//Change camera angle
		EnhancedInputComponent->BindAction(ChangeCamera, ETriggerEvent::Triggered, this, &ALostV2Character::ChangeCameraAngle);

		// HUD visability
		EnhancedInputComponent->BindAction(HUDAction, ETriggerEvent::Triggered, this, &ALostV2Character::SetHUDVisability);
	}
}

float ALostV2Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHealthHUD();
	return DamageAmount;
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

void ALostV2Character::AddXP(float Value)
{
	if (Attributes && LostOverlay) {
		Attributes->AddXP(Value);
		LostOverlay->SetXPBarPercent(Attributes->GetXPPercent());
		LostOverlay->SetLevel(Attributes->GetLevel());
	}
}

void ALostV2Character::SetHUDVisible()
{
	if (LostOverlay && LostOverlay->GetVisibility() == ESlateVisibility::Hidden) {
		LostOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void ALostV2Character::SetHUDHidden()
{
	if (LostOverlay && LostOverlay->GetVisibility() == ESlateVisibility::Visible) {
		LostOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ALostV2Character::SetCameraZoomToBattleMode()
{
	UCharacterMovementComponent* CharacterMovementSpeed = GetCharacterMovement();
	if (CharacterMovementSpeed) {
		bIsCharacterInBattleMode = true;
		CharacterMovementSpeed->MaxWalkSpeed = SprintSpeed;
		// Smoothly change the camera boom arm length to 200
		// If the character is moving it wiil be able to zoom the camera
		if (IsCharacterMoving(CharacterMovementSpeed)) { // if the character IS moving and holding "Sprint" key the CameraBoom arm lenght will be zoom 
			CountCanemraLenghtBoolsForZoom++;
			if (CountCanemraLenghtBoolsForZoom == 1) {
				CountCanemraLenghtBoolsForZoomOUT = 0;
				StartArmLength = 200.f;
				TargetArmLength = 300.f;
				bIsChangingArmLength = true;
				CurrentArmLengthTime = 0.0f;
			}
		}
	}
}

void ALostV2Character::SetCameraZoomToDefault()
{
	UCharacterMovementComponent* CharacterMovementSpeed = GetCharacterMovement();
	if (CharacterMovementSpeed) {
		CharacterMovementSpeed->MaxWalkSpeed = WalkSpeed;
	}
	// Smoothly change the camera boom arm length back to default
	StartArmLength = CameraBoom->TargetArmLength;
	TargetArmLength = DefaultArmLength;

	CountCanemraLenghtBoolsForZoom = 0;
	CountCanemraLenghtBoolsForZoomOUT = 0;

	// Initialize arm length interpolation variables
	bIsChangingArmLength = true;
	CurrentArmLengthTime = 0.0f;
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

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ALostV2Character::OnOverlapBegin);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &ALostV2Character::OnOverlapEnd);
}

void ALostV2Character::Die()
{
	ActionState = EActionState::EAS_Dead;
	LostDeathPose = ELostDeathPose::ELDP_Death1;

	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);

	Tags.Add(FName("Dead"));

	PlayMontage(DeathMontage, FName("Death1"));
	
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

void ALostV2Character::Move(const FInputActionValue& Value)
{
	if (bIsResting) {
		bIsResting = false;
		PlayMontage(HealMontage, FName("Standing"));
	}

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
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}

void ALostV2Character::StartSprinting()
{
	UCharacterMovementComponent* CharacterMovementSpeed = GetCharacterMovement();
	if (CharacterMovementSpeed) {
		CharacterMovementSpeed->MaxWalkSpeed = SprintSpeed;
		// Smoothly change the camera boom arm length to 200
		// If the character is moving it wiil be able to zoom the camera
		if (IsCharacterMoving(CharacterMovementSpeed)) { // if the character IS moving and holding "Sprint" key the CameraBoom arm lenght will be zoom 
			CountCanemraLenghtBoolsForZoom++;
			if (CountCanemraLenghtBoolsForZoom == 1) {
				CountCanemraLenghtBoolsForZoomOUT = 0;
				StartArmLength = 200.f;
				TargetArmLength = 300.f;
				bIsChangingArmLength = true;
				CurrentArmLengthTime = 0.0f;
			}
		}
		else if(!IsCharacterMoving(CharacterMovementSpeed)) { // if the character is NOT moving but still holding the "Sprint" key the camera will zoom out 
			CountCanemraLenghtBoolsForZoomOUT++;
			if (CountCanemraLenghtBoolsForZoomOUT == 1) {
				CountCanemraLenghtBoolsForZoom = 0;
				StartArmLength = 300.f;
				TargetArmLength = 200.f;
				bIsChangingArmLength = true;
				CurrentArmLengthTime = 0.0f;
				return;
			}
		}
	}
}

void ALostV2Character::StopSprinting()
{
	if (!bIsCharacterInBattleMode) {
		CountCanemraLenghtBoolsForZoom = 0;
		CountCanemraLenghtBoolsForZoomOUT = 0;
		UCharacterMovementComponent* CharacterMovementSpeed = GetCharacterMovement();
		if (CharacterMovementSpeed)
		{
			// Set the maximum walking speed to walking speed
			CharacterMovementSpeed->MaxWalkSpeed = WalkSpeed;
			if (CameraBoom) { // if the character is not moving then it will reset the camera boom
				// Set up the start and target arm lengths
				StartArmLength = CameraBoom->TargetArmLength;
				TargetArmLength = DefaultArmLength;

				// Initialize arm length interpolation variables
				bIsChangingArmLength = true;
				CurrentArmLengthTime = 0.0f;
			}
		}
	}
}

bool ALostV2Character::IsCharacterMoving(UCharacterMovementComponent* CharacterMovementSpeed)
{
	return CharacterMovementSpeed->Velocity.SizeSquared() > FMath::Square(0.1f);
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

void ALostV2Character::EKeyPressed()
{ 
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon && !EquippedWeapon) {
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this , this);
		CharacterState = ECharacterState::ECS_EquipedOneHandedWeapon;
		if (Attributes) {
			Attributes->AddDamage(OverlappingWeapon->GetDamage());
		}
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

	AController* CharacterController = GetController();
	ABonfire* OverlappingBonfire = Cast<ABonfire>(OverlappingItem);
	if (OverlappingBonfire && CharacterController) {
		if (!OverlappingBonfire->IsBonfireActive()) {
			FVector BonfireLocation = OverlappingBonfire->GetBonfireLocation();
			FRotator TargetRotation = (BonfireLocation - GetActorLocation()).Rotation();
			// Keep only the Yaw rotation (rotation around the vertical axis)
			FRotator NewRotation = FRotator(0.f, TargetRotation.Yaw, 0.f);
			// Set the actor's rotation
			SetActorRotation(NewRotation);
			PlayMontage(ReachOutMontage, FName("ReachOut"));
			return;
		}
		
		if (OverlappingBonfire->IsBonfireActive()) {
			OverlappingBonfire->InteractWidget->HideInteractText();
			Heal();
			return;
		}
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
	if (HealMontage && IsActionStateUnoccupied()) {
		LostRestingPose = ELostRestingPose::ELRP_Resting1;
		PlayMontage(HealMontage, FName("Sitting"));
		ActionState = EActionState::EAS_Resting;
	}
}

void ALostV2Character::ChangeCameraAngle()
{
	if (CameraBoom) {
		FVector ChangedSocketOffset;

		// Check the current socket offset value
		if (CameraBoom->SocketOffset.Y > 0.0f)
		{
			ChangedSocketOffset = FVector(0.0f, -60.0f, 25.f); // If positive, set to negative
		}
		else
		{
			ChangedSocketOffset = FVector(0.0f, 60.0f, 25.f); // If negative or zero, set to positive
		}

		// Set the target socket offset
		TargetSocketOffset = ChangedSocketOffset;

		// Set up start and target socket offsets for interpolation
		StartSocketOffset = CameraBoom->SocketOffset;

		// Initialize interpolation variables
		CurrentInterpolationTime = 0.0f;
		bIsChangingSocketOffset = true;
	}
}

void ALostV2Character::SetHUDVisability()
{
	// TODO: proper if statement
	if (LostOverlay) {
		if (LostOverlay->GetVisibility() == ESlateVisibility::Hidden) {
			LostOverlay->SetVisibility(ESlateVisibility::Visible);
		}
		else if (LostOverlay->GetVisibility() == ESlateVisibility::Visible) {
			LostOverlay->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

bool ALostV2Character::CanChangeCameraBoomSocketOffset()
{
	return bIsChangingSocketOffset;
}

bool ALostV2Character::CanChangeCameraBoomArmLenght()
{
	return bIsChangingArmLength;
}

void ALostV2Character::ChangeCameraBoomArmLenght(float DeltaTime)
{
	// Calculate the interpolation alpha for arm length
	float ArmLengthAlpha = FMath::Clamp(CurrentArmLengthTime / CameraBoomSmoothTransition, 0.0f, 1.0f);

	// Interpolate between the start and target arm length
	float NewArmLength = FMath::Lerp(StartArmLength, TargetArmLength, ArmLengthAlpha);
	
	// Update the arm length of the camera boom
	if (CameraBoom) {
		CameraBoom->TargetArmLength = NewArmLength;
	}

	// Update the current arm length time
	CurrentArmLengthTime += DeltaTime;

	// Check if the arm length interpolation is finished
	if (CurrentArmLengthTime >= 1.0f)
	{
		bIsChangingArmLength = false;
	}
}

void ALostV2Character::ChangeCameraBoomSocketOffset(float DeltaTime)
{
	// Calculate interpolation alpha
	float InterpolationAlpha = FMath::Clamp(CurrentInterpolationTime / SocketOffsetInterpolationTime, 0.0f, 1.0f);

	// Interpolate between the start and target socket offset
	FVector NewSocketOffset = FMath::Lerp(StartSocketOffset, TargetSocketOffset, InterpolationAlpha);

	// Update the socket offset of the camera boom
	if (CameraBoom)
	{
		CameraBoom->SocketOffset = NewSocketOffset;
	}

	// Update the current interpolation time
	CurrentInterpolationTime += DeltaTime;

	// Check if the interpolation is finished
	if (CurrentInterpolationTime >= SocketOffsetInterpolationTime)
	{
		bIsChangingSocketOffset = false;
	}
}

bool ALostV2Character::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();;
}

bool ALostV2Character::IsCharacterStateUnoccupied()
{
	return CharacterState == ECharacterState::ECS_Unequipped;
}

bool ALostV2Character::IsActionStateUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ALostV2Character::PlayMontage(UAnimMontage* Montage, FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

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

void ALostV2Character::StartBonfireEffect()
{
	ABonfire* OverlappingBonfire = Cast<ABonfire>(OverlappingItem);
	if (OverlappingBonfire) {
		OverlappingBonfire->ActivateBonfire();
	}
}

void ALostV2Character::StartHealing()
{
	bIsResting = true;
}

void ALostV2Character::StartResting()
{
	bCanMove = false;
}

void ALostV2Character::StopResting()
{
	ActionState = EActionState::EAS_Unoccupied;
	bCanMove = true;
}

void ALostV2Character::CharacterCollisionOnWhenDodging()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
}

void ALostV2Character::CharacterCollisionOFFWhenDodging()
{
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
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


void ALostV2Character::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If the character is overlapping with even a single enemy 
	// the character is going into battle mode where the camera zooms out, the HUD is set to visible if its not
	// and the character movement speed will be set to battle mode where it CAN'T walk
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy) {
		NumEnemiesInSphere++;
		if (NumEnemiesInSphere == 1) {
			SetCameraZoomToBattleMode();
			SetHUDVisible();
		}
	}
}

void ALostV2Character::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// If the character is NOT overlapping with a single enemy 
	// the Character is going out of battle mode where the camera goes to his default value if its not sprinting, the HUD will be hidden if its not
	// and the character can stop sprinting
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy) {
		NumEnemiesInSphere--;
		if (NumEnemiesInSphere == 0){
			GetWorldTimerManager().SetTimer(TimerHandle_ZoomOutAndHideHUD, this, &ALostV2Character::SetCameraZoomToDefaultAndHideHUDDelayed, ZoomOutAndHUDHiddenDelay, false);
		}
	}
}

void ALostV2Character::SetCameraZoomToDefaultAndHideHUDDelayed()
{
	bIsCharacterInBattleMode = false;
	SetCameraZoomToDefault();
	SetHUDHidden();
}



void ALostV2Character::InitializeLostOverlay(APlayerController* PlayerController)
{
	ALostHUD* LostHUD = Cast<ALostHUD>(PlayerController->GetHUD());
	if (LostHUD) {
		LostOverlay = LostHUD->GetLostOverlay();
		if (LostOverlay) {
			LostOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
			LostOverlay->SetStaminaBarPercent(1.f);
			LostOverlay->SetXPBarPercent(Attributes->GetXPPercent());
			LostOverlay->SetGold(0);
			LostOverlay->SetGold(0);
			LostOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ALostV2Character::SetHealthHUD()
{
	if (LostOverlay && Attributes) {
		LostOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}