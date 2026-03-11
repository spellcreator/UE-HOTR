// 


#include "AbilitySystem/Abilities/Player/HR_Fireblast.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/HRTags.h"


UHR_Fireblast::UHR_Fireblast()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Способность активируется через GameplayEvent с TargetData (ActorArray)
	// Тег триггера задаётся в Blueprint (AbilityTriggers) или можно задать здесь:
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = HRTags::HRAbilities::FireBlast;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

// ═════════════════════════════════════════════════════════════════════════════

void UHR_Fireblast::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// ── 1. Извлекаем таргет ──────────────────────────────────────────────
	AActor* Target = ExtractTargetActor(TriggerEventData);
	if (!IsValid(Target))
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] No valid target in EventData"), *GetName());
		EndAbilitySafe(true);
		return;
	}

	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!IsValid(Avatar))
	{
		EndAbilitySafe(true);
		return;
	}

	// ── 2. Проверяем дальность ───────────────────────────────────────────
	if (!IsTargetInRange(Avatar, Target))
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Target %s out of range (%.0f > %.0f)"),
			*GetName(), *Target->GetName(),
			FVector::Dist2D(Avatar->GetActorLocation(), Target->GetActorLocation()),
			AbilityRange);
		EndAbilitySafe(true);
		return;
	}

	// ── 3. Проверяем конус / разворот ────────────────────────────────────
	if (!IsTargetInFacingCone(Avatar, Target))
	{
		// Враг в ренже, но не в конусе — разворачиваемся
		RotateTowardsTarget(Avatar, Target);

#if !UE_BUILD_SHIPPING
		if (bDrawDebugs && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
				FString::Printf(TEXT("[%s] Rotating to face target"), *GetName()));
		}
#endif
	}

	// ── 4. Кэшируем таргет ──────────────────────────────────────────────
	CachedTarget = Target;

	// ── 5. Подписываемся на DamageNotify ─────────────────────────────────
	UAbilityTask_WaitGameplayEvent* DamageEvent =
		WaitGameplayEvent(HRTags::HRAbilities::Notify::DamageNotify, /*bOnlyTriggerOnce=*/ false);
	DamageEvent->EventReceived.AddDynamic(this, &ThisClass::OnDamageNotify);
	DamageEvent->ReadyForActivation();

	// ── 6. Монтаж ────────────────────────────────────────────────────────
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
		// Без монтажа — сразу наносим урон и дебафф
		ApplyHitDamage(Target);
		ApplyDebuff(Target);
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

	// Мгновенный урон от попадания
	ApplyHitDamage(Target);

	// Дебафф (DoT) — отдельный Duration GE
	ApplyDebuff(Target);
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

AActor* UHR_Fireblast::ExtractTargetActor(const FGameplayEventData* TriggerEventData) const
{
	if (!TriggerEventData)
		return nullptr;

	// Сначала проверяем Target напрямую (простой путь)
	if (IsValid(TriggerEventData->Target.Get()))
		return const_cast<AActor*>(TriggerEventData->Target.Get());

	// Затем пробуем TargetData (ActorArray)
	if (TriggerEventData->TargetData.IsValid(0))
	{
		const FGameplayAbilityTargetData* Data = TriggerEventData->TargetData.Get(0);
		if (Data)
		{
			TArray<TWeakObjectPtr<AActor>> Actors = Data->GetActors();
			if (Actors.Num() > 0 && Actors[0].IsValid())
			{
				return Actors[0].Get();
			}
		}
	}

	return nullptr;
}

bool UHR_Fireblast::IsTargetInRange(const AActor* Avatar, const AActor* Target) const
{
	if (AbilityRange <= 0.f) return true; // 0 = без ограничения

	const float Distance2D = FVector::Dist2D(Avatar->GetActorLocation(), Target->GetActorLocation());
	return Distance2D <= AbilityRange;
}

bool UHR_Fireblast::IsTargetInFacingCone(const AActor* Avatar, const AActor* Target) const
{
	const FVector Forward = Avatar->GetActorForwardVector().GetSafeNormal2D();
	const FVector ToTarget = (Target->GetActorLocation() - Avatar->GetActorLocation()).GetSafeNormal2D();

	if (ToTarget.IsNearlyZero()) return true; // Цель на нас — считаем что в конусе

	const float Dot = FVector::DotProduct(Forward, ToTarget);
	const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(FacingHalfAngleDeg));

	return Dot >= CosHalfAngle;
}

void UHR_Fireblast::RotateTowardsTarget(AActor* Avatar, const AActor* Target) const
{
	if (!IsValid(Avatar) || !IsValid(Target)) return;

	const FVector Direction = (Target->GetActorLocation() - Avatar->GetActorLocation()).GetSafeNormal2D();
	if (Direction.IsNearlyZero()) return;

	const FRotator NewRotation = FRotator(0.f, Direction.Rotation().Yaw, 0.f);
	Avatar->SetActorRotation(NewRotation);

	// Также обновляем ControlRotation чтобы камера и контроллер были синхронизированы
	if (APawn* Pawn = Cast<APawn>(Avatar))
	{
		if (AController* Controller = Pawn->GetController())
		{
			FRotator ControlRot = Controller->GetControlRotation();
			ControlRot.Yaw = NewRotation.Yaw;
			Controller->SetControlRotation(ControlRot);
		}
	}
}

void UHR_Fireblast::ApplyHitDamage(AActor* Target)
{
	// Используем базовый ApplyDamage из UHR_CombatAbility
	// Он применяет DamageEffect (Instant GE) с SetByCaller из DamageProfile
	ApplyDamage(Target);
}

void UHR_Fireblast::ApplyDebuff(AActor* Target)
{
	if (!DebuffEffect || !IsValid(Target)) return;

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!TargetASC) return;

	UAbilitySystemComponent* InstigatorASC = GetAbilitySystemComponentFromActorInfo();
	if (!InstigatorASC) return;

	// Создаём контекст
	FGameplayEffectContextHandle Context = InstigatorASC->MakeEffectContext();
	Context.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	// Создаём спеку дебаффа
	FGameplayEffectSpecHandle Spec = InstigatorASC->MakeOutgoingSpec(
		DebuffEffect, GetAbilityLevel(), Context);
	if (!Spec.IsValid()) return;

	// Передаём урон за тик через SetByCaller
	// Используем отдельный тег для дебаффа, или фоллбэк на DamageProfile.SetByCallerTag
	const FGameplayTag Tag = DebuffDamageSetByCallerTag.IsValid()
		? DebuffDamageSetByCallerTag
		: DamageProfile.SetByCallerTag;

	if (Tag.IsValid())
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			Spec, Tag, -DebuffDamagePerTick);
	}

	// Применяем к таргету
	InstigatorASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
}