#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core//HR_GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "HR_CombatAbility.generated.h"

// ─── Профиль урона ────────────────────────────────────────────────────────────
// Всё что нужно для нанесения урона описывается здесь.
// Настраивается в Blueprint-дефолтах способности.
USTRUCT(BlueprintType)
struct HEROESOFTHEREARGUARD_API FHR_DamageProfile
{
    GENERATED_BODY()

    // Базовый урон
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage", meta=(ClampMin="0"))
    float BaseDamage = 25.f;

    // Радиус хитбокса (для ApplyAOE). 0 = только точечный урон.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage", meta=(ClampMin="0"))
    float HitboxRadius = 150.f;

    // Смещение хитбокса вперёд от персонажа
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
    float ForwardOffset = 0.f;

    // Смещение хитбокса по высоте
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
    float ElevationOffset = 0.f;

    // SetByCaller тег для передачи урона в GE (по умолчанию — ближний бой игрока)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
    FGameplayTag SetByCallerTag;
};

/**
 * UHR_CombatAbility
 *
 * Базовый класс для боевых способностей, наносящих урон.
 * Содержит DamageEffect + DamageProfile и хелперы для нанесения урона:
 *   ApplyDamage(Target)    — точечный удар по одному актору
 *   ApplyAOE()             — AOE по всем противникам в радиусе хитбокса
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

    // ─── Helpers ──────────────────────────────────────────────────────────

    // Нанести урон конкретному актору
    UFUNCTION(BlueprintCallable, Category="Ability|Combat")
    void ApplyDamage(AActor* Target);

    // Найти акторов в хитбоксе и нанести урон всем
    // RadiusOverride < 0 → использует DamageProfile.HitboxRadius
    UFUNCTION(BlueprintCallable, Category="Ability|Combat")
    void ApplyAOE(float RadiusOverride = -1.f);

    // Найти акторов в хитбоксе (без нанесения урона — для визуала и проверок)
    UFUNCTION(BlueprintCallable, Category="Ability|Combat")
    TArray<AActor*> FindTargetsInAOE(float Radius = -1.f) const;

private:
    // Применяет DamageEffect с SetByCaller к одному ASC
    void ApplyDamageEffect(UAbilitySystemComponent* TargetASC);
};