#include "AbilitySystem/Abilities/Player/HR_Ability_Attack_JumpAttack.h"

#include "GameplayTags/HRTags.h"
#include "Tasks/AbilityTask_JumpToLocation.h"

UHR_Ability_Attack_JumpAttack::UHR_Ability_Attack_JumpAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = HRTags::HRAbilities::JumpAttack;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}

void UHR_Ability_Attack_JumpAttack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    FVector TargetLocation;
    if (!ExtractTargetLocation(TriggerEventData, TargetLocation))
    {
        EndAbilitySafe(true);
        return;
    }
    CachedTargetLocation = TargetLocation;
    
    DamageEvent();
    JumpStart();
    
}

void UHR_Ability_Attack_JumpAttack::OnJumpCompleted()
{
    
    if (UAnimInstance* AnimInstance = GetCurrentActorInfo()->AnimInstance.Get())
    {
        AnimInstance->Montage_JumpToSection(FName("JumpLand"), JumpMontage);
    }
    // Прыжок завершён — запускаем монтаж приземления.
    // DamageNotify уже слушается с ActivateAbility — ничего дополнительно не нужно.
    if (!JumpMontage)
    {
        ApplyAOE();
        EndAbilitySafe(false);
        return;
    }
    
}

void UHR_Ability_Attack_JumpAttack::OnJumpInterrupted()
{
    EndAbilitySafe(true);
}

void UHR_Ability_Attack_JumpAttack::OnMontageCompleted()
{
    EndAbilitySafe(false);
}


void UHR_Ability_Attack_JumpAttack::OnDamageNotify(FGameplayEventData /*Payload*/)
{
    ApplyAOE();
}

void UHR_Ability_Attack_JumpAttack::JumpStart()
{
    JumpTask = UAbilityTask_JumpToLocation::JumpToLocation(
        this, CachedTargetLocation, FlightDuration, ArcStrength, JumpHeightCurve, nullptr);
    JumpTask->OnCompleted.AddDynamic(this, &ThisClass::OnJumpCompleted);
    JumpTask->OnInterrupted.AddDynamic(this, &ThisClass::OnJumpInterrupted);
    JumpTask->ReadyForActivation();
    
    MontageTask = PlayMontage(JumpMontage, 1.f, FName("JumpStart"));
    MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
    MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
    MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnJumpInterrupted);
    MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnJumpInterrupted);
    MontageTask->ReadyForActivation();
    
}

void UHR_Ability_Attack_JumpAttack::DamageEvent()
{
    UAbilityTask_WaitGameplayEvent* DamageEvent =
        WaitGameplayEvent(HRTags::HRAbilities::Notify::DamageNotify);
    DamageEvent->EventReceived.AddDynamic(this, &ThisClass::OnDamageNotify);
    DamageEvent->ReadyForActivation();
}
