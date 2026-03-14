
#include "AbilitySystem/Core//HR_GameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayEffect.h"
#include "GamePlayTags/HRTags.h"

// ─────────────────────────────────────────────────────────────────────────────

UHR_GameplayAbility::UHR_GameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UHR_GameplayAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#if !UE_BUILD_SHIPPING
    if (bDrawDebugs && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
            FString::Printf(TEXT("[Ability] %s Activated"), *GetName()));
    }
#endif
}

bool UHR_GameplayAbility::TryStartFromInput(UHR_AbilitySystemComponent* ASC, const FGameplayTag& AbilityTag,
    UHR_AbilityTargetingComponent* TargetingComp, UHR_UnitTargetingComponent* UnitTargetComp)
{
    return false;
}

// ─── Монтаж ──────────────────────────────────────────────────────────────────
// ReadyForActivation() НЕ вызывается внутри.
// Правильный порядок: Create → Bind delegates → ReadyForActivation()

void UHR_GameplayAbility::SetTriggerTag(const FGameplayTag& Tag)
{
    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = Tag;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}

UAbilityTask_PlayMontageAndWait* UHR_GameplayAbility::PlayMontage(
    UAnimMontage* Montage,
    float Rate,
    FName StartSection,
    bool bStopWhenAbilityEnds)
{
    if (!Montage) return nullptr;

    return UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, NAME_None, Montage, Rate, StartSection, bStopWhenAbilityEnds);
}

// ─── События ─────────────────────────────────────────────────────────────────
// ReadyForActivation() НЕ вызывается внутри.
// Правильный порядок: Create → Bind delegates → ReadyForActivation()

UAbilityTask_WaitGameplayEvent* UHR_GameplayAbility::WaitGameplayEvent(
    FGameplayTag EventTag,
    bool bOnlyTriggerOnce)
{
    return UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
        this, EventTag, nullptr, bOnlyTriggerOnce, true);
}

UAbilityTask_WaitGameplayEvent* UHR_GameplayAbility::ListenForDamageNotify(bool bOnlyOnce)
{
    UAbilityTask_WaitGameplayEvent* Task =
    WaitGameplayEvent(HRTags::HRAbilities::Notify::DamageNotify, bOnlyOnce);
    Task->ReadyForActivation();
    return Task;
}


// ─── Эффекты ─────────────────────────────────────────────────────────────────

FActiveGameplayEffectHandle UHR_GameplayAbility::ApplyEffectToSelf(
    TSubclassOf<UGameplayEffect> EffectClass, float Level)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC || !EffectClass) return FActiveGameplayEffectHandle();

    FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
    Context.AddSourceObject(GetAvatarActorFromActorInfo());

    FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(EffectClass, Level, Context);
    if (!Spec.IsValid()) return FActiveGameplayEffectHandle();

    return ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
}

void UHR_GameplayAbility::ApplyEffectToTarget(
    AActor* Target,
    TSubclassOf<UGameplayEffect> Effect,
    const TArray<FHR_SetByCallerParam>& SetByCallerParams)
{
    if (!Effect || !IsValid(Target)) return;

    UAbilitySystemComponent* TargetASC =
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
    if (!TargetASC) return;

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!SourceASC) return;

    FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
    Ctx.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

    FGameplayEffectSpecHandle Spec =
        SourceASC->MakeOutgoingSpec(Effect, GetAbilityLevel(), Ctx);
    if (!Spec.IsValid()) return;

    // Прокидываем все SetByCaller
    for (const FHR_SetByCallerParam& Param : SetByCallerParams)
    {
        if (Param.Tag.IsValid())
        {
            UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
                Spec, Param.Tag, -Param.Magnitude);
        }
    }

    SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
}

void UHR_GameplayAbility::ApplyEffectToTargets(
    const TArray<AActor*>& Targets,
    TSubclassOf<UGameplayEffect> Effect,
    const TArray<FHR_SetByCallerParam>& SetByCallerParams)
{
    for (AActor* Target : Targets)
    {
        ApplyEffectToTarget(Target, Effect, SetByCallerParams);
    }
}


// ─── Завершение ──────────────────────────────────────────────────────────────

void UHR_GameplayAbility::EndAbilitySafe(bool bWasCancelled)
{
    if (!IsActive()) return;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}

void UHR_GameplayAbility::CancelAllTasks()
{
    for (UGameplayTask* Task : ActiveTasks)
    {
        if (IsValid(Task)) Task->ExternalCancel();
    }
}
