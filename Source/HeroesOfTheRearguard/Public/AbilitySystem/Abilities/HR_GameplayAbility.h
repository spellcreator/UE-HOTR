// 

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
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
};
