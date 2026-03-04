
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HR_Targetable.generated.h"

class UAbilitySystemComponent;

UINTERFACE(MinimalAPI, Blueprintable)
class UHR_Targetable : public UInterface
{
	GENERATED_BODY()
};

/**
 * IHR_Targetable
 *
 * Интерфейс для всех акторов, которых можно взять в таргет.
 * Реализуйте на любом персонаже/NPC/объекте, который должен быть целью.
 */
class HEROESOFTHEREARGUARD_API IHR_Targetable
{
	GENERATED_BODY()

public:

	/** Можно ли сейчас взять в таргет (жив, не скрыт, не иммунен и т.д.) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanBeTargeted() const;

	/** Отображаемое имя для UI (над полоской HP и т.д.) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FText GetTargetDisplayName() const;

	/** ASC таргета — для чтения атрибутов (HP, Mana) в UI */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAbilitySystemComponent* GetTargetASC() const;

	/** Точка, к которой рисовать индикатор выделения */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetTargetIndicatorLocation() const;
};