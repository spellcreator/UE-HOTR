// 


#include "AbilitySystem/Abilities/HR_BaseTargetedAbility.h"

bool UHR_BaseTargetedAbility::TryGetTargetLocation(const FGameplayEventData* TriggerEventData,
	const FGameplayAbilitySpecHandle& Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo& ActivationInfo, FVector& OutLocation) 
{
	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		const FGameplayAbilityTargetData* Data = TriggerEventData->TargetData.Get(0);
		if (Data && Data->HasEndPoint())
		{
			OutLocation = Data->GetEndPoint();
			return true;
		}
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	return false;
}
