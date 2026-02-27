// HR_GameplayAbility.h
#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "HR_AbilityTargetingType.h"
#include "HR_GameplayAbility.generated.h"

// ─── Targeting config (без изменений, совместимость с компонентом прицеливания) ──
USTRUCT(BlueprintType)
struct FAbilityTargetingData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    EHR_AbilityTargetingType TargetingType = EHR_AbilityTargetingType::Instant;

    UPROPERTY(EditDefaultsOnly, meta=(EditCondition="TargetingType != EHR_AbilityTargetingType::Instant"))
    float AOERadius = 0.f;

    UPROPERTY(EditDefaultsOnly, meta=(EditCondition="TargetingType != EHR_AbilityTargetingType::Instant"))
    float AbilityMaxRange = 0.f;

    UPROPERTY(EditDefaultsOnly, meta=(EditCondition="TargetingType != EHR_AbilityTargetingType::Instant"))
    float AbilityMinRange = 0.f;

    UPROPERTY(EditDefaultsOnly,meta=(EditCondition="TargetingType != EHR_AbilityTargetingType::Instant"))
    TObjectPtr<UMaterialInterface> DecalMaterial = nullptr;
};


/**
 * UHR_GameplayAbility
 *
 * Базовый класс для всех способностей проекта.
 * Содержит переиспользуемые хелперы для монтажа, событий, эффектов и завершения.
 * Не содержит игровой логики — только инфраструктуру.
 */
UCLASS(Abstract)
class HEROESOFTHEREARGUARD_API UHR_GameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    bool RequiresTargeting() const
    {
        return TargetingData.TargetingType != EHR_AbilityTargetingType::Instant;
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Debug")
    bool bDrawDebugs = false;

    UPROPERTY(EditDefaultsOnly, Category="Crash|Targeting")
    FAbilityTargetingData TargetingData;

protected:

    /*virtual void OnNotifyReceived(const FGameplayEventData& Data);*/
    // ─── Монтаж ───────────────────────────────────────────────────────────
    // Запускает монтаж через AbilityTask и возвращает задачу для биндинга колбэков.
    // Вызывать ReadyForActivation() на возвращённой задаче НЕ нужно — уже сделано внутри.
    UFUNCTION(BlueprintCallable, Category="Ability|Montage", meta=(HidePin="this"))
    UAbilityTask_PlayMontageAndWait* PlayMontage(
        UAnimMontage* Montage,
        float Rate = 1.f,
        FName StartSection = NAME_None,
        bool bStopWhenAbilityEnds = true);

    // ─── События ──────────────────────────────────────────────────────────
    // Подписывается на GameplayEvent и возвращает задачу для биндинга.
    UFUNCTION(BlueprintCallable, Category="Ability|Events", meta=(HidePin="this"))
    UAbilityTask_WaitGameplayEvent* WaitGameplayEvent(
        FGameplayTag EventTag,
        bool bOnlyTriggerOnce = false);

    // ─── Эффекты ──────────────────────────────────────────────────────────
    // Применяет GE к себе (self-buff, cooldown, cost и т.д.)
    UFUNCTION(BlueprintCallable, Category="Ability|Effects")
    FActiveGameplayEffectHandle ApplyEffect(
        TSubclassOf<UGameplayEffect> EffectClass,
        float Level = 1.f);

    // Применяет GE к массиву целей (урон, дебафф и т.д.)
    UFUNCTION(BlueprintCallable, Category="Ability|Effects")
    void ApplyEffectToTargets(
        TSubclassOf<UGameplayEffect> EffectClass,
        const TArray<AActor*>& Targets,
        float Level = 1.f);

    // ─── Извлечение цели из EventData ─────────────────────────────────────
    // Используется способностями, которые запускаются через GameplayEvent с TargetData.
    // Только C++ — UHT не разрешает экспозить const FGameplayEventData* в Blueprint.
    bool ExtractTargetLocation(
        const FGameplayEventData* TriggerEventData,
        FVector& OutLocation) const;

    // ─── Завершение ───────────────────────────────────────────────────────
    // Безопасное завершение: проверяет IsActive() перед вызовом EndAbility.
    UFUNCTION(BlueprintCallable, Category="Ability|Lifecycle")
    void EndAbilitySafe(bool bWasCancelled = false);

    // Отменяет все активные AbilityTask этой способности.
    UFUNCTION(BlueprintCallable, Category="Ability|Lifecycle")
    void CancelAllTasks();
};