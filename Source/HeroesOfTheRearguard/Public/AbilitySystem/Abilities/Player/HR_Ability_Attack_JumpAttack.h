#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/HR_CombatAbility.h"
#include "HR_Ability_Attack_JumpAttack.generated.h"

class UCurveFloat;
class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_JumpToLocation;


/**
 * UHR_Ability_Attack_JumpAttack
 *
 * Прыжок на цель → при приземлении: монтаж + DamageNotify → ApplyAOE.
 * Наследуется от UHR_CombatAbility → всё управление уроном через DamageProfile.
 * ExtractTargetLocation берётся из базового UHR_GameplayAbility.
 *
 * Что задать в Blueprint:
 *   - FlightDuration, ArcStrength, JumpHeightCurve
 *   - LandingMontage
 *   - DamageEffect, DamageProfile
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_Ability_Attack_JumpAttack : public UHR_CombatAbility
{
	GENERATED_BODY()

public:
	UHR_Ability_Attack_JumpAttack();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Crash|JumpAttack|Flight")
	float FlightDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category="Crash|JumpAttack|Flight", meta=(ClampMin="0", ClampMax="1000"))
	float ArcStrength = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category="Crash|JumpAttack|Flight")
	TObjectPtr<UCurveFloat> JumpHeightCurve;

	UPROPERTY(EditDefaultsOnly, Category="Crash|JumpAttack|Animation")
	TObjectPtr<UAnimMontage> JumpMontage;

	
	UPROPERTY(BlueprintReadOnly)
	FVector CachedTargetLocation;

private:
	UFUNCTION() void OnJumpCompleted();
	UFUNCTION() void OnJumpInterrupted();
	UFUNCTION() void OnMontageCompleted();
	//UFUNCTION() void OnMontageInterrupted();
	UFUNCTION() void OnDamageNotify(FGameplayEventData Payload);
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
	UPROPERTY()
	TObjectPtr<UAbilityTask_JumpToLocation> JumpTask;
	
	void JumpStart();
	void DamageEvent();
};