
#include "AbilitySystem/Core//HR_GameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayEffect.h"

// ─────────────────────────────────────────────────────────────────────────────

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

// ─── Монтаж ──────────────────────────────────────────────────────────────────
// ReadyForActivation() НЕ вызывается внутри.
// Правильный порядок: Create → Bind delegates → ReadyForActivation()

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

// ─── Эффекты ─────────────────────────────────────────────────────────────────

FActiveGameplayEffectHandle UHR_GameplayAbility::ApplyEffect(
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

void UHR_GameplayAbility::ApplyEffectToTargets(
    TSubclassOf<UGameplayEffect> EffectClass,
    const TArray<AActor*>& Targets,
    float Level)
{
    if (!EffectClass || Targets.IsEmpty()) return;

    UAbilitySystemComponent* InstigatorASC = GetAbilitySystemComponentFromActorInfo();
    if (!InstigatorASC) return;

    FGameplayEffectContextHandle Context = InstigatorASC->MakeEffectContext();
    Context.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

    FGameplayEffectSpecHandle Spec = InstigatorASC->MakeOutgoingSpec(EffectClass, Level, Context);
    if (!Spec.IsValid()) return;

    for (AActor* Target : Targets)
    {
        if (!IsValid(Target)) continue;

        if (UAbilitySystemComponent* TargetASC =
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
        {
            InstigatorASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
        }
    }
}

// ─── Извлечение цели ─────────────────────────────────────────────────────────

bool UHR_GameplayAbility::ExtractTargetLocation(
    const FGameplayEventData* TriggerEventData, FVector& OutLocation) const
{
    if (!TriggerEventData || !TriggerEventData->TargetData.IsValid(0)) return false;

    const FGameplayAbilityTargetData* Data = TriggerEventData->TargetData.Get(0);
    if (!Data || !Data->HasEndPoint()) return false;

    OutLocation = Data->GetEndPoint();
    return true;
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
