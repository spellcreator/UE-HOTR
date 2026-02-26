// 


#include "AbilitySystem/Abilities/Player/HR_Ability_Attack_JumpAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/HR_BaseCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTags/HRTags.h"
#include "Kismet/GameplayStatics.h"
#include "Tasks/AbilityTask_JumpToLocation.h"
#include "Utils/HR_BlueprintLibrary.h"

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

    if (!TriggerEventData || !TriggerEventData->TargetData.IsValid(0))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    const FGameplayAbilityTargetData* Data = TriggerEventData->TargetData.Get(0);
    if (!Data || !Data->HasEndPoint())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UAbilityTask_JumpToLocation* JumpTask = UAbilityTask_JumpToLocation::JumpToLocation(
        this,
        Data->GetEndPoint(),
        FlightDuration,
        ArcStrength,
        JumpHeightCurve,
        nullptr
    );
    JumpTask->OnCompleted.AddDynamic(this, &ThisClass::OnJumpCompleted);
    JumpTask->OnInterrupted.AddDynamic(this, &ThisClass::OnJumpInterrupted);
    JumpTask->ReadyForActivation();
    
    // ── 2. Запускаем монтаж ───────────────────────────────────────────────
    if (!LandingMontage)
    {
        // Нет монтажа — урон сразу и выходим
        ApplyLandingDamage();
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }
    UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this,
        NAME_None,
        LandingMontage,
        1.0f,
        NAME_None,
        true
    );
    MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
    MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
    MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
    MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
    MontageTask->ReadyForActivation();
}

void UHR_Ability_Attack_JumpAttack::OnJumpCompleted()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Jump completed"));
    // ── 1. Слушаем AnimNotify ──────────────────────────────────────────────
    UAbilityTask_WaitGameplayEvent* WaitDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
        this,
        HRTags::HRAbilities::Notify::DamageNotify  // тег из твоего AnimNotify
    );
    WaitDamage->EventReceived.AddDynamic(this, &ThisClass::OnDamageNotify);
    WaitDamage->ReadyForActivation();
    
    
}

void UHR_Ability_Attack_JumpAttack::OnJumpInterrupted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UHR_Ability_Attack_JumpAttack::OnMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UHR_Ability_Attack_JumpAttack::OnMontageInterrupted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UHR_Ability_Attack_JumpAttack::OnDamageNotify(FGameplayEventData /*Payload*/)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnDamgeNotify"));
    ApplyLandingDamage();
}

void UHR_Ability_Attack_JumpAttack::ApplyLandingDamage()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Damaged"));
    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (!IsValid(AvatarActor) || !DamageEffect) return;

    TArray<AActor*> HitActors = UHR_BlueprintLibrary::HitboxOverlapTest(
        AvatarActor, DamageHitboxRadius, 0.f, 0.f, bDrawDebugs);

    if (HitActors.IsEmpty()) return;

    UAbilitySystemComponent* InstigatorASC = GetAbilitySystemComponentFromActorInfo();
    if (!InstigatorASC) return;

    FGameplayEffectContextHandle Context = InstigatorASC->MakeEffectContext();
    Context.AddInstigator(AvatarActor, AvatarActor);

    FGameplayEffectSpecHandle Spec = InstigatorASC->MakeOutgoingSpec(
        DamageEffect, GetAbilityLevel(), Context);
    if (!Spec.IsValid()) return;

    for (AActor* Target : HitActors)
    {
        if (!IsValid(Target)) continue;
        if (UAbilitySystemComponent* TargetASC =
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
        {
            InstigatorASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
        }
    }
}