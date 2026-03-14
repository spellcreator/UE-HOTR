// 


#include "AbilitySystem/Core/HR_UnitTargetAbility.h"

#include "AbilitySystem/HR_AbilitySystemComponent.h"
#include "Player/HR_UnitTargetingComponent.h"

UHR_UnitTargetAbility::UHR_UnitTargetAbility()
{
	TargetingData.TargetingType = EHR_AbilityTargetingType::UnitTarget;
}

void UHR_UnitTargetAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Извлечение таргета и кеш аватара ────────────────────────────────────
	CachedTarget = ExtractTargetActor(TriggerEventData);
	if (!IsValid(CachedTarget.Get()))
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
	// Проверяем дальность ────────────────────────────────────
	if (!IsTargetInRange(Avatar, CachedTarget.Get(),TargetingData.AbilityMaxRange ))
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Target %s out of range (%.0f > %.0f)"),
			*GetName(), *CachedTarget->GetName(),
			FVector::Dist2D(Avatar->GetActorLocation(), CachedTarget->GetActorLocation()),
			TargetingData.AbilityMaxRange);
		EndAbilitySafe(true);
		return;
	}
	// Проверяем конус / разворот ────────────────────────────────────
	if (!IsTargetInFacingCone(Avatar, CachedTarget.Get(), TargetingData.ConeHalfAngleDeg))
	{
		// Враг в ренже, но не в конусе — разворачиваемся
		RotateTowardsTarget(Avatar, CachedTarget.Get());

#if !UE_BUILD_SHIPPING
		if (bDrawDebugs && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
				FString::Printf(TEXT("[%s] Rotating to face target"), *GetName()));
		}
#endif
	}
}

bool UHR_UnitTargetAbility::TryStartFromInput(UHR_AbilitySystemComponent* ASC, const FGameplayTag& AbilityTag,
	UHR_AbilityTargetingComponent* TargetingComp, UHR_UnitTargetingComponent* UnitTargetComp)
{
	if (!UnitTargetComp || !UnitTargetComp->HasTarget())
	{
		// Можно показать UI-подсказку
		return false;
	}

	AActor* Target = UnitTargetComp->GetCurrentTarget();

	FGameplayAbilityTargetData_ActorArray* ActorData = 
		new FGameplayAbilityTargetData_ActorArray();
	ActorData->TargetActorArray.Add(Target);

	FGameplayEventData EventData;
	EventData.Instigator = ASC->GetAvatarActor();
	EventData.Target = Target;
	EventData.TargetData.Add(ActorData);

	ASC->HandleGameplayEvent(AbilityTag, &EventData);
	return true;
}

AActor* UHR_UnitTargetAbility::ExtractTargetActor(const FGameplayEventData* TriggerEventData) const
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

bool UHR_UnitTargetAbility::IsTargetInRange(const AActor* Avatar, const AActor* Target, float AbilityMaxRange) const
{
	if (AbilityMaxRange <= 0.f) return true; // 0 = без ограничения

	const float Distance2D = FVector::Dist2D(Avatar->GetActorLocation(), Target->GetActorLocation());
	return Distance2D <= AbilityMaxRange;
}

bool UHR_UnitTargetAbility::IsTargetInFacingCone(const AActor* Avatar, const AActor* Target, float FacingHalfAngleDeg) const
{
	const FVector Forward = Avatar->GetActorForwardVector().GetSafeNormal2D();
	const FVector ToTarget = (Target->GetActorLocation() - Avatar->GetActorLocation()).GetSafeNormal2D();

	if (ToTarget.IsNearlyZero()) return true; // Цель на нас — считаем что в конусе

	const float Dot = FVector::DotProduct(Forward, ToTarget);
	const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(FacingHalfAngleDeg));

	return Dot >= CosHalfAngle;
}

void UHR_UnitTargetAbility::RotateTowardsTarget(AActor* Avatar, const AActor* Target) const
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