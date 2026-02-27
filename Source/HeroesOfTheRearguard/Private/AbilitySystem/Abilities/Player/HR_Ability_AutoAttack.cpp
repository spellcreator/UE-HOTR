#include "AbilitySystem/Abilities/Player/HR_Ability_AutoAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/HRTags.h"

UHR_Ability_AutoAttack::UHR_Ability_AutoAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

// ─────────────────────────────────────────────────────────────────────────────

void UHR_Ability_AutoAttack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!AttackMontage)
    {
        ApplyAOE();
        EndAbilitySafe(false);
        return;
    }

    // ── 1. Сначала биндим делегаты, потом ReadyForActivation ────────────
    UAbilityTask_PlayMontageAndWait* MontageTask = PlayMontage(AttackMontage);
    MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
    MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
    MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
    MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
    MontageTask->ReadyForActivation();

    // ── 2. Сначала биндим делегаты, потом ReadyForActivation ────────────
    UAbilityTask_WaitGameplayEvent* DamageEvent =
        WaitGameplayEvent(HRTags::HRAbilities::Notify::DamageNotify);
    DamageEvent->EventReceived.AddDynamic(this, &ThisClass::OnDamageNotify);
    DamageEvent->ReadyForActivation();
}

void UHR_Ability_AutoAttack::OnDamageNotify(FGameplayEventData /*Payload*/)
{
    TArray<AActor*> HitActors = FindTargetsInAOE();
    for (AActor* Target : HitActors)
    {
        ApplyDamage(Target);
    }
    SendHitReactEvents(HitActors);
}

void UHR_Ability_AutoAttack::OnMontageCompleted()
{
    EndAbilitySafe(false);
}

void UHR_Ability_AutoAttack::OnMontageInterrupted()
{
    EndAbilitySafe(true);
}

void UHR_Ability_AutoAttack::SendHitReactEvents(const TArray<AActor*>& ActorsHit)
{
    FGameplayEventData Payload;
    Payload.Instigator = GetAvatarActorFromActorInfo();

    for (AActor* HitActor : ActorsHit)
    {
		
        FGameplayEventData PayLoad;
        PayLoad.Instigator = GetAvatarActorFromActorInfo();
		
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, HRTags::Events::Enemy::HitReact,PayLoad );
    }
}