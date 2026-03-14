// 

#pragma once

#include "CoreMinimal.h"
#include "HR_GameplayAbility.h"
#include "HR_GroundTargetAbility.generated.h"

/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_GroundTargetAbility : public UHR_GameplayAbility
{
	GENERATED_BODY()
public:
	UHR_GroundTargetAbility();
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual bool TryStartFromInput(UHR_AbilitySystemComponent* ASC, const FGameplayTag& AbilityTag, UHR_AbilityTargetingComponent* TargetingComp, UHR_UnitTargetingComponent* UnitTargetComp) override;
	
	bool ExtractTargetLocation(
		const FGameplayEventData* TriggerEventData,
		FVector& OutLocation) const;
};
