// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/HR_UnitTargetAbility.h"
#include "HR_Fireblast.generated.h"

class UHR_CombatModule;
/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_Fireblast : public UHR_UnitTargetAbility
{
	GENERATED_BODY()
public:
	UHR_Fireblast();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	
	// ── Combat Logic ──────────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Combat")
	TObjectPtr<UHR_CombatModule> CombatModule;
	
	// ── Анимация ──────────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|FireBlast")
	TObjectPtr<UAnimMontage> AbilityMontage;


private:
	// ── Callbacks ─────────────────────────────────────────────────────────

	UFUNCTION() void OnDamageNotify(FGameplayEventData Payload);
	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageInterrupted();

	// ── Helpers ───────────────────────────────────────────────────────────
	
};
