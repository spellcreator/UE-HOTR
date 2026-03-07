// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/HR_CombatAbility.h"
#include "HR_Ability_Combat_Spark.generated.h"

/**
 * UHR_Ability_Combat_Spark
 *
 * Направленная способность (DirectionalArc): конусный удар перед персонажем.
 * Параметры конуса задаются в DamageProfile (HitboxShape = Cone).
 * 
 * Поток:
 *   1. Игрок нажимает клавишу → начинается прицеливание (DirectionalArc декаль).
 *   2. Игрок подтверждает → PlayerController отправляет GameplayEvent с TargetLocation.
 *   3. Способность извлекает направление, поворачивает персонажа.
 *   4. Монтаж → DamageNotify → ApplyAOE() (использует Cone из DamageProfile).
 *
 * Что задать в Blueprint:
 *   - DamageProfile.HitboxShape    = Cone
 *   - DamageProfile.ConeRange      = дальность
 *   - DamageProfile.ConeHalfAngleDeg = половина угла
 *   - DamageEffect                 = GE с SetByCaller
 *   - TargetingData.TargetingType  = DirectionalArc
 *   - SparkMontage                 = анимация каста
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_Ability_Combat_Spark : public UHR_CombatAbility
{
	GENERATED_BODY()

public:
	UHR_Ability_Combat_Spark();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Spark|Animation")
	TObjectPtr<UAnimMontage> SparkMontage;

private:
	UFUNCTION() void OnDamageNotify(FGameplayEventData Payload);
	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageInterrupted();

	void RotateOwnerToDirection(AActor* Avatar, const FVector& Direction) const;
};