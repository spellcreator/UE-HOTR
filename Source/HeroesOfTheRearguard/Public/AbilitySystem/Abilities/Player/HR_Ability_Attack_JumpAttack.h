// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HR_BaseTargetedAbility.h"
#include "AbilitySystem/Abilities/HR_GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "HR_Ability_Attack_JumpAttack.generated.h"


class AHR_BaseCharacter;
/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_Ability_Attack_JumpAttack : public UHR_BaseTargetedAbility
{
	GENERATED_BODY()
public:
	UHR_Ability_Attack_JumpAttack();
    
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	// Прыжок
	UPROPERTY(EditDefaultsOnly, Category="HR Ability Attack Jump Attack")
	float FlightDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category="HR Ability Attack Jump Attack", meta=(ClampMin="0", ClampMax="1000"))
	float ArcStrength = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category="HR Ability Attack Jump Attack")
	TObjectPtr<UCurveFloat> JumpHeightCurve;

	// Анимация
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TObjectPtr<UAnimMontage> LandingMontage;

	// Урон
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamageAmount = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamageHitboxRadius = 300.f;

	// Колбэки
	UFUNCTION() void OnJumpCompleted();
	UFUNCTION() void OnJumpInterrupted();
	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageInterrupted();
	UFUNCTION() void OnDamageNotify(FGameplayEventData Payload);

	void ApplyLandingDamage();
};
