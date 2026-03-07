#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core//HR_GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "HR_HitboxShape.h"
#include "HR_CombatAbility.generated.h"

// ─── Профиль урона ────────────────────────────────────────────────────────────
// Всё что нужно для нанесения урона описывается здесь.
// Настраивается в Blueprint-дефолтах способности.
USTRUCT(BlueprintType)
struct HEROESOFTHEREARGUARD_API FHR_DamageProfile
{
    GENERATED_BODY()

    // ─── Общее ────────────────────────────────────────────────────────────
    
    // Базовый урон
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage", meta=(ClampMin="0"))
    float BaseDamage = 25.f;

    // Форма хитбокса
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
    EHR_HitboxShape HitboxShape = EHR_HitboxShape::Sphere;

    // Смещение хитбокса вперёд от персонажа
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
    float ForwardOffset = 0.f;

    // Смещение хитбокса по высоте
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
    float ElevationOffset = 0.f;

    // SetByCaller тег для передачи урона в GE (по умолчанию — ближний бой игрока)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
    FGameplayTag SetByCallerTag;

    // ─── Sphere ───────────────────────────────────────────────────────────
    
    // Радиус сферы (для Sphere-хитбокса). 0 = только точечный урон.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage|Sphere",
        meta=(ClampMin="0", EditCondition="HitboxShape == EHR_HitboxShape::Sphere"))
    float HitboxRadius = 150.f;

    // ─── Cone ─────────────────────────────────────────────────────────────
    
    // Дальность конуса
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage|Cone",
        meta=(ClampMin="0", EditCondition="HitboxShape == EHR_HitboxShape::Cone"))
    float ConeRange = 600.f;

    // Половина угла конуса в градусах (45 = конус 90°)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage|Cone",
        meta=(ClampMin="5", ClampMax="180", EditCondition="HitboxShape == EHR_HitboxShape::Cone"))
    float ConeHalfAngleDeg = 45.f;

    // ─── Box ──────────────────────────────────────────────────────────────
    
    // Полуразмеры бокса (X = вперёд, Y = вбок, Z = вверх)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage|Box",
        meta=(EditCondition="HitboxShape == EHR_HitboxShape::Box"))
    FVector BoxHalfExtent = FVector(100.f, 100.f, 100.f);
};

/**
 * UHR_CombatAbility
 *
 * Базовый класс для боевых способностей, наносящих урон.
 * Содержит DamageEffect + DamageProfile и хелперы для нанесения урона:
 *   ApplyDamage(Target)    — точечный удар по одному актору
 *   ApplyAOE()             — AOE по всем противникам в хитбоксе
 *
 * Для направленных способностей (Cone/Box):
 *   Задайте HitboxDirectionOverride перед вызовом ApplyAOE().
 *   Если ZeroVector — используется ActorForwardVector.
 */
UCLASS(Abstract)
class HEROESOFTHEREARGUARD_API UHR_CombatAbility : public UHR_GameplayAbility
{
    GENERATED_BODY()

protected:

    // GE, которое применяется при ударе. Должен содержать SetByCaller модификатор урона.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Combat|Damage")
    TSubclassOf<UGameplayEffect> DamageEffect;

    // Параметры удара: урон, хитбокс, теги
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Combat|Damage")
    FHR_DamageProfile DamageProfile;

    /**
     * Направление хитбокса (для Cone и Box).
     * ZeroVector = использовать ActorForwardVector.
     * Задавать перед вызовом ApplyAOE() / FindTargetsInHitbox().
     */
    UPROPERTY(BlueprintReadWrite, Category="Crash|Combat|Runtime")
    FVector HitboxDirectionOverride = FVector::ZeroVector;

    // ─── Helpers ──────────────────────────────────────────────────────────

    // Нанести урон конкретному актору
    UFUNCTION(BlueprintCallable, Category="Ability|Combat")
    void ApplyDamage(AActor* Target);

    // Найти акторов в хитбоксе и нанести урон всем
    UFUNCTION(BlueprintCallable, Category="Ability|Combat")
    void ApplyAOE();

    // Найти акторов в хитбоксе (без нанесения урона — для визуала и проверок)
    UFUNCTION(BlueprintCallable, Category="Ability|Combat")
    TArray<AActor*> FindTargetsInHitbox() const;

private:
    // Применяет DamageEffect с SetByCaller к одному ASC
    void ApplyDamageEffect(UAbilitySystemComponent* TargetASC);

    // Получить актуальное направление (override или ActorForward)
    FVector GetHitboxDirection() const;
};