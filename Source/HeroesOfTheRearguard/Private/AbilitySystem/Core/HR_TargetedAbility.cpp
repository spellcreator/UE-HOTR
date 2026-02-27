#include "AbilitySystem/Core/HR_TargetedAbility.h"

#include "Abilities/GameplayAbilityTargetTypes.h"

bool UHR_TargetedAbility::ValidateTargetData(const FGameplayEventData* TriggerEventData) const
{
	if (!TriggerEventData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] ValidateTargetData: TriggerEventData is null"), *GetName());
		return false;
	}

	if (!TriggerEventData->TargetData.IsValid(0))
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] ValidateTargetData: TargetData is invalid"), *GetName());
		return false;
	}

	const FGameplayAbilityTargetData* Data = TriggerEventData->TargetData.Get(0);
	if (!Data || !Data->HasEndPoint())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] ValidateTargetData: No endpoint in TargetData"), *GetName());
		return false;
	}

	return true;
}


