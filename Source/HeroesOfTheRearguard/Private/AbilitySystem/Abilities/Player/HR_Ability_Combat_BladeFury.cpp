// 


#include "AbilitySystem/Abilities/Player/HR_Ability_Combat_BladeFury.h"

#include "Tasks/AbilityTask_PeriodicAction.h"

void UHR_Ability_Combat_BladeFury::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	
	auto* Task = UAbilityTask_PeriodicAction::PeriodicAction(
		this, DamagePeriod, AbilityDuration, /*bFireImmediately=*/ bOnActivated);
	Task->OnPeriodicAction.AddDynamic(this, &ThisClass::OnPeriodicDamage);
	Task->OnFinished.AddDynamic(this, &ThisClass::OnDurationExpired);
	Task->ReadyForActivation();
	
	MontageStart();
}

void UHR_Ability_Combat_BladeFury::OnPeriodicDamage()
{
	ApplyAOE();
}

void UHR_Ability_Combat_BladeFury::OnDurationExpired()
{
	EndAbilitySafe(false);
}

void UHR_Ability_Combat_BladeFury::MontageStart()
{
	/*MontageTask = PlayMontage(FuryMontage);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnJumpInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnJumpInterrupted);
	MontageTask->ReadyForActivation();*/
}


void UHR_Ability_Combat_BladeFury::OnMontageInterrupted()
{
	EndAbilitySafe(true);
}

void UHR_Ability_Combat_BladeFury::ApplyDamage()
{
}

void UHR_Ability_Combat_BladeFury::OnMontageCompleted()
{
	EndAbilitySafe(false);
}
