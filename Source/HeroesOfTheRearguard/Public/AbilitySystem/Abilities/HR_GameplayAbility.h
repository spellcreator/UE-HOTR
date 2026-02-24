// 

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HR_AbilityTargetingType.h"
#include "HR_GameplayAbility.generated.h"


/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Debugs")
	bool bDrawDebugs = false;
	
	// ── Targeting ──────────────────────────────────────────
    
	/** Тип прицеливания. Instant = активировать сразу */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Targeting")
	EHR_AbilityTargetingType TargetingType = EHR_AbilityTargetingType::Instant;

	/** Радиус AOE (для GroundTarget / DirectionalArc) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Targeting",
			  meta=(EditCondition="TargetingType != EHR_AbilityTargetingType::Instant"))
	float AOERadius = 300.f;

	/** Дальность прицеливания */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Targeting",
			  meta=(EditCondition="TargetingType != EHR_AbilityTargetingType::Instant"))
	float TargetingMaxRange = 1200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Targeting",
			  meta=(EditCondition="TargetingType != EHR_AbilityTargetingType::Instant"))
	float TargetingMinRange = 0.f;

	bool RequiresTargeting() const
	{
		return TargetingType != EHR_AbilityTargetingType::Instant;
	}
	
	// Материал декали для этой конкретной способности
	UPROPERTY(EditDefaultsOnly, Category="Crash|Targeting",
			  meta=(EditCondition="TargetingType != EHR_AbilityTargetingType::Instant"))
	TObjectPtr<UMaterialInterface> TargetingDecalMaterial;
};
