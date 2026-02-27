#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/HR_GameplayAbility.h"
#include "HR_TargetedAbility.generated.h"

/**
 * UHR_TargetedAbility
 *
 * Базовый класс для способностей, которые требуют указания цели перед активацией
 * (GroundTarget, DirectionalArc и т.д.).
 *
 * Поверх UHR_GameplayAbility добавляет:
 *   ValidateTargetData()        — проверить, что TargetData из EventData корректны
 *   ExtractTargetLocation()     — уже есть в базе, здесь override с логом
 *
 * Наследники должны подписываться на триггер через GameplayEvent (TriggerSource::GameplayEvent)
 * и получать TargetData от PlayerController через HandleGameplayEvent.
 */
UCLASS(Abstract)
class HEROESOFTHEREARGUARD_API UHR_TargetedAbility : public UHR_GameplayAbility
{
	GENERATED_BODY()

protected:
	// Проверяет наличие корректной TargetData в EventData.
	// Вызвать в начале ActivateAbility; при false — сделать EndAbilitySafe(true).
	// Только C++ — UHT не разрешает экспозить const FGameplayEventData* в Blueprint.
	bool ValidateTargetData(const FGameplayEventData* TriggerEventData) const;
};


