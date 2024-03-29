#pragma once

// ECHO ENUMS
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquipedOneHandedWeapon UMETA(DisplayName = "Equiped One-Handed Weapon"),
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayName = "EAS_HitReaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Dodging UMETA(DisplayName = "Dodging"),
	EAS_Resting UMETA(DisplayName = "Resting"),
	EAS_Dead UMETA(DisplayName = "Dead"),
};

UENUM(BlueprintType)
enum class ELostDeathPose : uint8
{
	ELDP_Death1 UMETA(DisplayName = "Death1"),
};

UENUM(BlueprintType)
enum class ELostRestingPose : uint8
{
	ELRP_Resting1 UMETA(DisplayName = "Resting1"),
};


// ENEMY ENUMS

UENUM(BlueprintType)
enum class EDeathPose : uint8 
{
	EDP_Alive UMETA(DisplayName = "Alive"),
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA (DisplayName = "Attacking"),
	EES_NoState UMETA (DisplayName = "NoState")
};


UENUM(BlueprintType)
enum class EEnemyStateAtThatMoment : uint8
{
	EES_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EES_Occupied UMETA(DisplayName = "Occupied"),
};
