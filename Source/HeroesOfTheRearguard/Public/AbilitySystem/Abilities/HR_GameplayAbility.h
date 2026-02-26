// 

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HR_AbilityTargetingType.h"
#include "HR_GameplayAbility.generated.h"


USTRUCT(BlueprintType)
struct FAbilityTargetingData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EHR_AbilityTargetingType TargetingType;

	UPROPERTY(EditDefaultsOnly)
	float AOERadius = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float AbilityMaxRange = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float AbilityMinRange = 0.f;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> DecalMaterial = nullptr;
};


UCLASS()
class HEROESOFTHEREARGUARD_API UHR_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Debugs")
	bool bDrawDebugs = false;

	bool RequiresTargeting() const
	{
		return TargetingData.TargetingType != EHR_AbilityTargetingType::Instant;
	}
	
	
	UPROPERTY(EditDefaultsOnly)
	FAbilityTargetingData TargetingData;
};
