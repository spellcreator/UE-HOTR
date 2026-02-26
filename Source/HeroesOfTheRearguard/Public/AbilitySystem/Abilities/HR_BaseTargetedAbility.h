// 

#pragma once

#include "CoreMinimal.h"
#include "HR_GameplayAbility.h"
#include "HR_BaseTargetedAbility.generated.h"

/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_BaseTargetedAbility : public UHR_GameplayAbility
{
	GENERATED_BODY()
public:
	bool TryGetTargetLocation(
		const FGameplayEventData* TriggerEventData,
		const FGameplayAbilitySpecHandle& Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo& ActivationInfo,
		FVector& OutLocation);
};
