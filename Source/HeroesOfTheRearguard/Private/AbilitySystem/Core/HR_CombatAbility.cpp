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

void UHR_CombatAbility::ApplyAOE(float RadiusOverride)
{
    TArray<AActor*> Targets = FindTargetsInAOE(RadiusOverride);
    for (AActor* Target : Targets)
    {
        ApplyDamage(Target);
    }
}

TArray<AActor*> UHR_CombatAbility::FindTargetsInAOE(float Radius) const
{
    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!IsValid(Avatar)) return {};

    const float FinalRadius = Radius >= 0.f ? Radius : TargetingData.AOERadius;

    return UHR_BlueprintLibrary::HitboxOverlapTest(
        Avatar,
        FinalRadius,
        DamageProfile.ForwardOffset,
        DamageProfile.ElevationOffset,
        bDrawDebugs
    );
}

// ─── Private ─────────────────────────────────────────────────────────────────

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

    // Передаём величину урона через SetByCaller
    const FGameplayTag Tag = DamageProfile.SetByCallerTag.IsValid()
        ? DamageProfile.SetByCallerTag
        : HRTags::SetByCaller::PlayerMelee;

    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
        Spec, Tag, -DamageProfile.BaseDamage);

    InstigatorASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
}