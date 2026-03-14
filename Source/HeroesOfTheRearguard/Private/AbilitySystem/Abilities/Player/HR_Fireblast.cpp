// 


#include "AbilitySystem/Abilities/Player/HR_Fireblast.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Core/HR_CombatModule.h"
#include "GameplayTags/HRTags.h"


UHR_Fireblast::UHR_Fireblast()
{
	SetTriggerTag(HRTags::HRAbilities::FireBlast);
}

// ═════════════════════════════════════════════════════════════════════════════

void UHR_Fireblast::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// Подписываемся на DamageNotify ─────────────────────────────────
	auto* DmgTask = ListenForDamageNotify();
	DmgTask->EventReceived.AddDynamic(this, &ThisClass::OnDamageNotify);

	// Монтаж ────────────────────────────────────────────────────────
	if (AbilityMontage)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = PlayMontage(AbilityMontage);
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->ReadyForActivation();
	}
	else
	{
		CombatModule->ApplyDamage(GetAbilitySystemComponentFromActorInfo(),
						   CachedTarget.Get(), GetAbilityLevel());
		
		EndAbilitySafe(false);
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// Callbacks
// ═════════════════════════════════════════════════════════════════════════════

void UHR_Fireblast::OnDamageNotify(FGameplayEventData /*Payload*/)
{
	AActor* Target = CachedTarget.Get();
	if (!IsValid(Target))
	{
		EndAbilitySafe(true);
		return;
	}
	CombatModule->ApplyDamage(GetAbilitySystemComponentFromActorInfo(),
							   CachedTarget.Get(), GetAbilityLevel());
	EndAbilitySafe(false);

}

void UHR_Fireblast::OnMontageCompleted()
{
	EndAbilitySafe(false);
}

void UHR_Fireblast::OnMontageInterrupted()
{
	EndAbilitySafe(true);
}

// ═════════════════════════════════════════════════════════════════════════════
// Helpers
// ═════════════════════════════════════════════════════════════════════════════