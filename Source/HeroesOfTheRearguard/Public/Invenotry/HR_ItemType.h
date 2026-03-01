// 

#pragma once

UENUM(BlueprintType)
enum class EHR_ItemType : uint8
{
	Weapon UMETA(DisplayName = "Weapon"),
	Armor UMETA(DisplayName = "Armor"),
	Craft UMETA(DisplayName = "Craft"),
	Consumable UMETA(DisplayName = "Consumable"),
};