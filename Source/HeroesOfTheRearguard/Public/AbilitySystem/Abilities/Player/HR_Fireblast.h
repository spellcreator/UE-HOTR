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
	
	// New Combat Logic
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Combat")
	TObjectPtr<UHR_CombatModule> CombatModule;
	// ── Анимация ──────────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|UnitTargeted|Animation")
	TObjectPtr<UAnimMontage> AbilityMontage;
	
	// ── Дальность и конус ─────────────────────────────────────────────────

	/** Максимальная дальность до цели (в юнитах). Если 0 — без ограничения. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|UnitTargeted|Range")
	float AbilityRange = 800.f;

	/** Половина угла конуса перед персонажем (45 = конус 90°).
	 *  Если враг внутри — кастуем сразу. Если снаружи — разворачиваемся. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|UnitTargeted|Range",
		meta=(ClampMin="5", ClampMax="180"))
	float FacingHalfAngleDeg = 45.f;
	


private:
	// ── Callbacks ─────────────────────────────────────────────────────────

	UFUNCTION() void OnDamageNotify(FGameplayEventData Payload);
	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageInterrupted();

	// ── Helpers ───────────────────────────────────────────────────────────
	
};
