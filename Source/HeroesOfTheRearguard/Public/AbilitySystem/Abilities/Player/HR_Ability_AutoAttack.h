#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/HR_CombatAbility.h"
#include "AbilitySystem/Core/HR_GameplayAbility.h"
#include "HR_Ability_AutoAttack.generated.h"

/**
 * UHR_LMBAbility
 *
 * Базовый ближний бой (LMB).
 * Логика: PlayMontage → ждём DamageNotify → ApplyAOE → ждём конца монтажа → End.
 *
 * Всё что нужно задать в Blueprint:
 *   - AttackMontage
 *   - DamageEffect
 *   - DamageProfile (урон, радиус, смещения)
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_Ability_AutoAttack : public UHR_CombatAbility
{
	GENERATED_BODY()

public:
	UHR_Ability_AutoAttack();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Crash|LMB|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	// Отправить событие HitReact всем поражённым актором (для анимации реакции врагов)
	UFUNCTION(BlueprintCallable, Category="Crash|Abilities")
	void SendHitReactEvents(const TArray<AActor*>& ActorsHit);

private:
	UFUNCTION() void OnDamageNotify(FGameplayEventData Payload);
	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageInterrupted();
};
