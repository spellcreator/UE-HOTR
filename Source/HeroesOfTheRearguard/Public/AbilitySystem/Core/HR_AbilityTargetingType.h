#pragma once

UENUM(BlueprintType)
enum class EHR_AbilityTargetingType : uint8
{
	Instant         UMETA(DisplayName = "Instant"),           // Сразу
	GroundTarget    UMETA(DisplayName = "Ground Target"),     // Круг на земле
	DirectionalArc  UMETA(DisplayName = "Directional Arc"),  // Конус/направление
	//UnitTarget      UMETA(DisplayName = "Unit Target"),       // Цель — актор
};