// 


#include "AbilitySystem/Core/HR_InstantAbility.h"

#include "AbilitySystem/HR_AbilitySystemComponent.h"

UHR_InstantAbility::UHR_InstantAbility()
{
	
}

bool UHR_InstantAbility::TryStartFromInput(UHR_AbilitySystemComponent* ASC, const FGameplayTag& AbilityTag,
	UHR_AbilityTargetingComponent* TargetingComp, UHR_UnitTargetingComponent* UnitTargetComp)
{
	return ASC->TryActivateAbilityByTag(AbilityTag);
}
