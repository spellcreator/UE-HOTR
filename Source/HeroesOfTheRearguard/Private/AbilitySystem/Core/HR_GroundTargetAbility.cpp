// 


#include "AbilitySystem/Core/HR_GroundTargetAbility.h"

#include "Player/HR_AbilityTargetingComponent.h"

UHR_GroundTargetAbility::UHR_GroundTargetAbility()
{
	TargetingData.TargetingType = EHR_AbilityTargetingType::GroundTarget;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UHR_GroundTargetAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool UHR_GroundTargetAbility::TryStartFromInput(UHR_AbilitySystemComponent* ASC, const FGameplayTag& AbilityTag,
	UHR_AbilityTargetingComponent* TargetingComp, UHR_UnitTargetingComponent* UnitTargetComp)
{
	if (!TargetingComp) return false;

	TargetingComp->BeginTargeting(AbilityTag, this);
	return true;
}

bool UHR_GroundTargetAbility::ExtractTargetLocation(const FGameplayEventData* TriggerEventData,
	FVector& OutLocation) const
{
	if (!TriggerEventData || !TriggerEventData->TargetData.IsValid(0)) return false;

	const FGameplayAbilityTargetData* Data = TriggerEventData->TargetData.Get(0);
	if (!Data || !Data->HasEndPoint()) return false;

	OutLocation = Data->GetEndPoint();
	return true;
}
