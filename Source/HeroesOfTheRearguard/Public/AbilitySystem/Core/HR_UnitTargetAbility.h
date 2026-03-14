// 

#pragma once

#include "CoreMinimal.h"
#include "HR_GameplayAbility.h"
#include "HR_UnitTargetAbility.generated.h"

/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_UnitTargetAbility : public UHR_GameplayAbility
{
	GENERATED_BODY()
public:
	UHR_UnitTargetAbility();
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual bool TryStartFromInput(UHR_AbilitySystemComponent* ASC, const FGameplayTag& AbilityTag, UHR_AbilityTargetingComponent* TargetingComp, UHR_UnitTargetingComponent* UnitTargetComp) override;
	
protected:
	AActor* ExtractTargetActor(const FGameplayEventData* TriggerEventData) const;
	bool IsTargetInRange(const AActor* Avatar, const AActor* Target, float AbilityMaxRange) const;
	bool IsTargetInFacingCone(const AActor* Avatar, const AActor* Target, float FacingHalfAngleDeg) const;
	void RotateTowardsTarget(AActor* Avatar, const AActor* Target) const;

	UPROPERTY()
	TWeakObjectPtr<AActor> CachedTarget;
};
