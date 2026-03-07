#include "AbilitySystem/Abilities/Player/HR_Ability_AutoAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/HRTags.h"

UHR_Ability_AutoAttack::UHR_Ability_AutoAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

// ─────────────────────────────────────────────────────────────────────────────

/*
void UHR_Ability_AutoAttack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
}
*/
