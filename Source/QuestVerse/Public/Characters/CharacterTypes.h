#pragma once

UENUM(BlueprintType)
enum class ECharacterState :uint8
{
	ECS_Unequipped UMETA(DispalyName = "Unequipped"),
	ECS_EquippedBow UMETA(DispalyName = "Equipped Bow"),
	ECS_EquippedGreatSword UMETA(DispalyName = "Equipped GreatSword"),
	ECS_EquippedSpear UMETA(DispalyName = "Equipped Spear"),
};