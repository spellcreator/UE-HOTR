#include "AbilitySystem/Core/HR_CombatAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/HRTags.h"
#include "Utils/HR_BlueprintLibrary.h"

// ─────────────────────────────────────────────────────────────────────────────

void UHR_CombatAbility::ApplyDamage(AActor* Target)
{
    if (!IsValid(Target)) return;

    UAbilitySystemComponent* TargetASC =
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
    if (!TargetASC) return;

    ApplyDamageEffect(TargetASC);
}

void UHR_CombatAbility::ApplyAOE()
{
    TArray<AActor*> Targets = FindTargetsInHitbox();
    for (AActor* Target : Targets)
    {
        ApplyDamage(Target);
    }
}

TArray<AActor*> UHR_CombatAbility::FindTargetsInHitbox() const
{
    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!IsValid(Avatar)) return {};

    // Определяем радиус: для target-способностей берём из TargetingData, иначе из DamageProfile
    auto GetRadius = [&]() -> float
    {
        if (TargetingData.TargetingType != EHR_AbilityTargetingType::Instant
            && TargetingData.AOERadius > 0.f)
        {
            return TargetingData.AOERadius;
        }
        return DamageProfile.HitboxRadius;
    };

    const FVector Direction = GetHitboxDirection();

    return UHR_BlueprintLibrary::HitboxOverlapTest(
        Avatar,
        DamageProfile.HitboxShape,
        GetRadius(),
        DamageProfile.ForwardOffset,
        DamageProfile.ElevationOffset,
        Direction,
        DamageProfile.ConeHalfAngleDeg,
        DamageProfile.ConeRange,
        DamageProfile.BoxHalfExtent,
        bDrawDebugs
    );
}

// ─── Private ─────────────────────────────────────────────────────────────────

FVector UHR_CombatAbility::GetHitboxDirection() const
{
    if (!HitboxDirectionOverride.IsNearlyZero())
    {
        return HitboxDirectionOverride.GetSafeNormal2D();
    }

    const AActor* Avatar = GetAvatarActorFromActorInfo();
    if (IsValid(Avatar))
    {
        return Avatar->GetActorForwardVector().GetSafeNormal2D();
    }
    return FVector::ForwardVector;
}

void UHR_CombatAbility::ApplyDamageEffect(UAbilitySystemComponent* TargetASC)
{
    if (!TargetASC || !DamageEffect) return;

    UAbilitySystemComponent* InstigatorASC = GetAbilitySystemComponentFromActorInfo();
    if (!InstigatorASC) return;

    FGameplayEffectContextHandle Context = InstigatorASC->MakeEffectContext();
    Context.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

    FGameplayEffectSpecHandle Spec = InstigatorASC->MakeOutgoingSpec(
        DamageEffect, GetAbilityLevel(), Context);
    if (!Spec.IsValid()) return;

    const FGameplayTag Tag = DamageProfile.SetByCallerTag.IsValid()
        ? DamageProfile.SetByCallerTag
        : HRTags::SetByCaller::PlayerMelee;

    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
        Spec, Tag, -DamageProfile.BaseDamage);

    InstigatorASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
}