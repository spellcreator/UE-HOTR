// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/HR_CombatAbility.h"
#include "HR_Fireblast.generated.h"

/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_Fireblast : public UHR_CombatAbility
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
	// ── Дальность и конус ─────────────────────────────────────────────────

	/** Максимальная дальность до цели (в юнитах). Если 0 — без ограничения. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|UnitTargeted|Range")
	float AbilityRange = 800.f;

	/** Половина угла конуса перед персонажем (45 = конус 90°).
	 *  Если враг внутри — кастуем сразу. Если снаружи — разворачиваемся. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|UnitTargeted|Range",
		meta=(ClampMin="5", ClampMax="180"))
	float FacingHalfAngleDeg = 45.f;

	// ── Анимация ──────────────────────────────────────────────────────────

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|UnitTargeted|Animation")
	TObjectPtr<UAnimMontage> AbilityMontage;

	// ── Дебафф (DoT) ─────────────────────────────────────────────────────

	/** GE дебаффа. Должен быть Duration + Period.
	 *  Движок сам тикает урон каждые Period секунд.
	 *  Настрой Modifier с SetByCaller для передачи урона. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|UnitTargeted|Debuff")
	TSubclassOf<UGameplayEffect> DebuffEffect;

	/** Урон за один тик дебаффа (передаётся через SetByCaller). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|UnitTargeted|Debuff",
		meta=(ClampMin="0"))
	float DebuffDamagePerTick = 10.f;

	/** Тег SetByCaller для урона дебаффа. Если не задан — используется DamageProfile.SetByCallerTag. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|UnitTargeted|Debuff")
	FGameplayTag DebuffDamageSetByCallerTag;

private:
	// ── Callbacks ─────────────────────────────────────────────────────────

	UFUNCTION() void OnDamageNotify(FGameplayEventData Payload);
	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageInterrupted();

	// ── Helpers ───────────────────────────────────────────────────────────

	/** Извлекает таргет-актора из EventData (ActorArray[0]). */
	AActor* ExtractTargetActor(const FGameplayEventData* TriggerEventData) const;

	/** Проверяет расстояние до таргета. */
	bool IsTargetInRange(const AActor* Avatar, const AActor* Target) const;

	/** Проверяет, находится ли таргет в конусе перед персонажем. */
	bool IsTargetInFacingCone(const AActor* Avatar, const AActor* Target) const;

	/** Мгновенно разворачивает персонажа к цели. */
	void RotateTowardsTarget(AActor* Avatar, const AActor* Target) const;

	/** Применяет мгновенный урон к одному таргету. */
	void ApplyHitDamage(AActor* Target);

	/** Применяет дебафф (Duration GE) к одному таргету. */
	void ApplyDebuff(AActor* Target);

	// ── Cached data ──────────────────────────────────────────────────────

	UPROPERTY()
	TWeakObjectPtr<AActor> CachedTarget;
};
