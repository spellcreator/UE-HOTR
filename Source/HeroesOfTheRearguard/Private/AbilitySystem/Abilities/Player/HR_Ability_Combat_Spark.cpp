// 


#include "AbilitySystem/Abilities/Player/HR_Ability_Combat_Spark.h"

#include "AbilitySystem/Core/HR_CombatModule.h"
#include "GamePlayTags/HRTags.h"
#include "Utils/HR_BlueprintLibrary.h"

UHR_Ability_Combat_Spark::UHR_Ability_Combat_Spark()
{
	SetTriggerTag(HRTags::HRAbilities::DirectionalArc);
}

void UHR_Ability_Combat_Spark::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// ── 1. Извлекаем точку прицеливания ─────────────────────────────────
	FVector TargetLocation;
	if (!ExtractTargetLocation(TriggerEventData, TargetLocation))
	{
		EndAbilitySafe(true);
		return;
	}

	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!IsValid(Avatar))
	{
		EndAbilitySafe(true);
		return;
	}

	// ── 2. Вычисляем направление конуса ─────────────────────────────────
	FVector Dir = (TargetLocation - Avatar->GetActorLocation()).GetSafeNormal2D();
	if (Dir.IsNearlyZero())
	{
		Dir = Avatar->GetActorForwardVector().GetSafeNormal2D();
	}
	

	// ── 3. Поворачиваем персонажа ───────────────────────────────────────
	RotateOwnerToDirection(Avatar, Dir);

	// ── 4. Подписываемся на DamageNotify ────────────────────────────────
	UAbilityTask_WaitGameplayEvent* DamageEvent =
		WaitGameplayEvent(HRTags::HRAbilities::Notify::DamageNotify);
	DamageEvent->EventReceived.AddDynamic(this, &ThisClass::OnDamageNotify);
	DamageEvent->ReadyForActivation();

	// ── 5. Монтаж ───────────────────────────────────────────────────────
	if (SparkMontage)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = PlayMontage(SparkMontage);
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->ReadyForActivation();
	}
	else
	{
		CombatModule->ApplyAOE(
			GetAbilitySystemComponentFromActorInfo(),
			Avatar,
			Avatar->GetActorForwardVector(),  // Direction
			0.f,                               // OverrideRadius (0 = из DamageProfile)
			GetAbilityLevel(),
			bDrawDebugs);
		EndAbilitySafe(false);
	}
}

// ─── Callbacks ───────────────────────────────────────────────────────────────

void UHR_Ability_Combat_Spark::OnDamageNotify(FGameplayEventData /*Payload*/)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	CombatModule->ApplyAOE(
			GetAbilitySystemComponentFromActorInfo(),
			Avatar,
			Avatar->GetActorForwardVector(),  // Direction
			0.f,                               // OverrideRadius (0 = из DamageProfile)
			GetAbilityLevel());
}

void UHR_Ability_Combat_Spark::OnMontageCompleted()
{
	EndAbilitySafe(false);
}

void UHR_Ability_Combat_Spark::OnMontageInterrupted()
{
	EndAbilitySafe(true);
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

void UHR_Ability_Combat_Spark::RotateOwnerToDirection(AActor* Avatar, const FVector& Direction) const
{
	if (!IsValid(Avatar)) return;
	const FRotator Rot = Direction.Rotation();
	Avatar->SetActorRotation(FRotator(0.f, Rot.Yaw, 0.f));
}