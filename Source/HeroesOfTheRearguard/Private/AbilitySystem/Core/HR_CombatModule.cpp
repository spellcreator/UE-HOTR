// 


#include "AbilitySystem/Core/HR_CombatModule.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystem/Core/HR_GameplayAbility.h"
#include "GamePlayTags/HRTags.h"
#include "Utils/HR_BlueprintLibrary.h"

void UHR_CombatModule::ApplyDamage(UAbilitySystemComponent* SourceASC,
									AActor* Target, float Level)
{
	if (!IsValid(Target) || !SourceASC) return;

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!TargetASC) return;

	// Урон 
	ApplyDamageEffect(SourceASC, TargetASC, Level);

	// Дебаффы
	ApplyDebuffToTarget(SourceASC, TargetASC, Level);
}

void UHR_CombatModule::ApplyAOE(UAbilitySystemComponent* SourceASC, AActor* AvatarActor, const FVector& Direction,
float OverrideRadius, float Level, bool bDrawDebug)
{
	TArray<AActor*> Targets = FindTargetsInHitbox(
		AvatarActor, Direction, OverrideRadius, bDrawDebug);

	for (AActor* Target : Targets)
	{
		ApplyDamage(SourceASC, Target, Level);
	}
}

TArray<AActor*> UHR_CombatModule::FindTargetsInHitbox(AActor* AvatarActor, const FVector& Direction,
	float OverrideRadius, bool bDrawDebug) const
{
	if (!IsValid(AvatarActor)) return {};

	float Radius = OverrideRadius > 0.f 
		? OverrideRadius 
		: DamageProfile.HitboxRadius;

	return UHR_BlueprintLibrary::HitboxOverlapTest(
		AvatarActor,
		DamageProfile.HitboxShape,
		Radius,
		DamageProfile.ForwardOffset,
		DamageProfile.ElevationOffset,
		Direction,
		DamageProfile.ConeHalfAngleDeg,
		DamageProfile.ConeRange,
		DamageProfile.BoxHalfExtent,
		bDrawDebug 
	);
}

void UHR_CombatModule::ApplyDamageEffect(UAbilitySystemComponent* SourceASC,
	UAbilitySystemComponent* TargetASC, float Level)
{
	if (!TargetASC || !DamageEffect) return;

	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.AddInstigator(SourceASC->GetAvatarActor(), SourceASC->GetAvatarActor());

	FGameplayEffectSpecHandle Spec = 
		SourceASC->MakeOutgoingSpec(DamageEffect, Level, Ctx);
	if (!Spec.IsValid()) return;

	const FGameplayTag Tag = DamageProfile.SetByCallerTag.IsValid()
		? DamageProfile.SetByCallerTag
		: HRTags::SetByCaller::PlayerMelee;

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		Spec, Tag, -DamageProfile.BaseDamage);

	SourceASC->ApplyGameplayEffectSpecToTarget(
		*Spec.Data.Get(), TargetASC);
}

void UHR_CombatModule::ApplyDebuffToTarget(
	UAbilitySystemComponent* SourceASC,
	UAbilitySystemComponent* TargetASC,
	float Level)
{
	for (const FHR_DebuffEntry& Debuff : Debuffs)
	{
		if (!Debuff.Effect) continue;

		FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
		Ctx.AddInstigator(SourceASC->GetAvatarActor(), SourceASC->GetAvatarActor());

		FGameplayEffectSpecHandle Spec =
			SourceASC->MakeOutgoingSpec(Debuff.Effect, Level, Ctx);
		if (!Spec.IsValid()) continue;

		for (const FHR_SetByCallerParam& Param : Debuff.SetByCallerParams)
		{
			if (Param.Tag.IsValid())
			{
				UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
					Spec, Param.Tag, -Param.Magnitude);
			}
		}

		SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
	}
}

