// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/HR_InstantAbility.h"
#include "HR_Ability_Combat_BladeFury.generated.h"

class UHR_CombatModule;
/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_Ability_Combat_BladeFury : public UHR_InstantAbility
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Combat")
	TObjectPtr<UHR_CombatModule> CombatModule;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "BladeFury")
	float AbilityDuration = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "BladeFury")
	float DamagePeriod = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "BladeFury")
	bool bOnActivated = false;

private:
	UFUNCTION() void OnPeriodicDamage();
	UFUNCTION() void OnDurationExpired();
	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageInterrupted();
	
	void MontageStart();
	void ApplyDamage();
	
	
	
};
