// HR_GameplayAbility.h
#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Enums/HR_AbilityTargetingType.h"
#include "HR_GameplayAbility.generated.h"

// ─── Targeting config (без изменений, совместимость с компонентом прицеливания) ──
USTRUCT(BlueprintType)
struct FAbilityTargetingData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    EHR_AbilityTargetingType TargetingType = EHR_AbilityTargetingType::Instant;

    UPROPERTY(EditDefaultsOnly, meta=(
    EditCondition="TargetingType != EHR_AbilityTargetingType::Instant",
    EditConditionHides))
    float AOERadius = 0.f;

    UPROPERTY(EditDefaultsOnly, meta=(
    EditCondition="TargetingType != EHR_AbilityTargetingType::Instant",
    EditConditionHides))
    float AbilityMaxRange = 0.f;

    UPROPERTY(EditDefaultsOnly, meta=(
    EditCondition="TargetingType != EHR_AbilityTargetingType::Instant",
    EditConditionHides))
    float AbilityMinRange = 0.f;
    
    UPROPERTY(EditDefaultsOnly, meta=(EditCondition="TargetingType == EHR_AbilityTargetingType::DirectionalArc",
    EditConditionHides))
    float ConeHalfAngleDeg = 45.f;

    UPROPERTY(EditDefaultsOnly,meta=(EditCondition="TargetingType == EHR_AbilityTargetingType::GroundTarget",
    EditConditionHides))
    TObjectPtr<UMaterialInterface> DecalMaterial = nullptr;
};

// Один SetByCaller параметр для эффекта
USTRUCT(BlueprintType)
struct FHR_SetByCallerParam
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag Tag;

    UPROPERTY(EditDefaultsOnly)
    float Magnitude = 0.f;
};

class UHR_UnitTargetingComponent;
class UHR_AbilityTargetingComponent;
class UHR_AbilitySystemComponent;


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
    
    UHR_GameplayAbility();
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;
    
    /**
 * Вызывается контроллером при нажатии кнопки.
 * Способность сама решает: активироваться сразу, 
 * начать таргетинг, или потребовать юнит-цель.
 * 
 * Возвращает true если начала что-то делать.
 */
    virtual bool TryStartFromInput(
        UHR_AbilitySystemComponent* ASC,
        const FGameplayTag& AbilityTag,
        UHR_AbilityTargetingComponent* TargetingComp,
        UHR_UnitTargetingComponent* UnitTargetComp);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Debug")
    bool bDrawDebugs = false;

    UPROPERTY(EditDefaultsOnly, Category="Crash|Targeting")
    FAbilityTargetingData TargetingData;

protected:
    
    /** Регистрирует тег как триггер по GameplayEvent */
    void SetTriggerTag(const FGameplayTag& Tag);

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
    
    /** Подписаться на DamageNotify. Биндить колбэк на возвращённый таск. */
    UAbilityTask_WaitGameplayEvent* ListenForDamageNotify(bool bOnlyOnce = false);

    // ─── Эффекты ──────────────────────────────────────────────────────────
    // Применяет GE к себе (self-buff, cooldown, cost и т.д.)
    UFUNCTION(BlueprintCallable, Category="Ability|Effects")
    FActiveGameplayEffectHandle ApplyEffectToSelf(
        TSubclassOf<UGameplayEffect> EffectClass,
        float Level = 1.f);

    /** Применить GE к одному таргету с опциональными SetByCaller. */
    UFUNCTION(BlueprintCallable, Category="Ability|Effects")
    void ApplyEffectToTarget(
        AActor* Target,
    TSubclassOf<UGameplayEffect> Effect,
    const TArray<FHR_SetByCallerParam>& SetByCallerParams);

    /** Применить GE к массиву таргетов. */
    UFUNCTION(BlueprintCallable, Category="Ability|Effects")
    void ApplyEffectToTargets(
        const TArray<AActor*>& Targets,
        TSubclassOf<UGameplayEffect> Effect,
        const TArray<FHR_SetByCallerParam>& SetByCallerParams = {});

    // ─── Завершение ───────────────────────────────────────────────────────
    // Безопасное завершение: проверяет IsActive() перед вызовом EndAbility.
    UFUNCTION(BlueprintCallable, Category="Ability|Lifecycle")
    void EndAbilitySafe(bool bWasCancelled = false);

    // Отменяет все активные AbilityTask этой способности.
    UFUNCTION(BlueprintCallable, Category="Ability|Lifecycle")
    void CancelAllTasks();
};