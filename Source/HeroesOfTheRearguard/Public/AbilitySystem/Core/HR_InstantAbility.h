// 

#pragma once

#include "CoreMinimal.h"
#include "HR_GameplayAbility.h"
#include "HR_InstantAbility.generated.h"

/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_InstantAbility : public UHR_GameplayAbility
{
	GENERATED_BODY()
	
public:
	UHR_InstantAbility();
	
	virtual bool TryStartFromInput(UHR_AbilitySystemComponent* ASC, const FGameplayTag& AbilityTag, UHR_AbilityTargetingComponent* TargetingComp, UHR_UnitTargetingComponent* UnitTargetComp) override;
};
