// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "HR_AbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEROESOFTHEREARGUARD_API UHR_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRep_ActivateAbilities() override;
	UFUNCTION(BlueprintCallable, Category="Crash|Abilities")
	void SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level);
	UFUNCTION(BlueprintCallable, Category="Crash|Abilities")
	void AddAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level = 1);
	
private:
	
	void HandleAutoActivateAbilities(const FGameplayAbilitySpec& AbilitySpec);
};
