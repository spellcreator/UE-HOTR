// 

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_PeriodicAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPeriodicActionDelegate);

/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UAbilityTask_PeriodicAction : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable) FPeriodicActionDelegate OnPeriodicAction;
	UPROPERTY(BlueprintAssignable) FPeriodicActionDelegate OnFinished;
	
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks",
	meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UAbilityTask_PeriodicAction* PeriodicAction(
		UGameplayAbility* OwningAbility,
		float Period,          // интервал между вызовами
		float TotalDuration,   // общая длительность (<=0 = бесконечно)
		bool bFireImmediately = true); // первый вызов сразу или через Period
	
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	
private:
	float Period;
	float TotalDuration;
	bool  bFireImmediately;

	float ElapsedTime = 0.f;
	float TimeSinceLastAction = 0.f;
};


