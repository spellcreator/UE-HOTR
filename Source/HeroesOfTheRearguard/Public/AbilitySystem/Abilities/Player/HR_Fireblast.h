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
	

	// ── Дебафф (DoT) ─────────────────────────────────────────────────────

	/** GE дебаффа. Должен быть Duration + Period.
	 *  Движок сам тикает урон каждые Period секунд.
	 *  Настрой Modifier с SetByCaller для передачи урона. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Debuff")
	TSubclassOf<UGameplayEffect> DebuffEffect;

	/*/** Урон за один тик дебаффа (передаётся через SetByCaller). #1#
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Debuff",
		meta=(ClampMin="0"))
	float DebuffDamagePerTick = 10.f;

	/** Тег SetByCaller для урона дебаффа. Если не задан — используется DamageProfile.SetByCallerTag. #1#
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|UnitTargeted|Debuff")
	FGameplayTag DebuffDamageSetByCallerTag;*/
	
	UPROPERTY(EditDefaultsOnly, Category="Crash|Fireblast|Debuff")
	FHR_SetByCallerParam DebuffParam;

private:
	// ── Callbacks ─────────────────────────────────────────────────────────

	UFUNCTION() void OnDamageNotify(FGameplayEventData Payload);
	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageInterrupted();

	// ── Helpers ───────────────────────────────────────────────────────────
	
	
	// ── Cached data ──────────────────────────────────────────────────────

	UPROPERTY()
	TWeakObjectPtr<AActor> CachedTarget;
};
