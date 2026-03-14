// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Enums/HR_HitboxShape.h"
#include "UObject/Object.h"
#include "HR_CombatModule.generated.h"


class UGameplayEffect;
class UAbilitySystemComponent;
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
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew)
class HEROESOFTHEREARGUARD_API UHR_CombatModule : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FHR_DamageProfile DamageProfile;

	void ApplyDamage(UAbilitySystemComponent* SourceASC,
					 AActor* Target, float Level = 1.f);

	void ApplyAOE(UAbilitySystemComponent* SourceASC,
				  AActor* AvatarActor,
				  const FVector& Direction,
				  float OverrideRadius = 0.f,
				  float Level);

	TArray<AActor*> FindTargetsInHitbox(
				  AActor* AvatarActor,
				  const FVector& Direction,
				  float OverrideRadius = 0.f,
				  bool bDrawDebug = false) const;
	
private:
	// Применяет DamageEffect с SetByCaller к одному ASC
	void ApplyDamageEffect(UAbilitySystemComponent* SourceASC,
	UAbilitySystemComponent* TargetASC, float Level);
	
};
