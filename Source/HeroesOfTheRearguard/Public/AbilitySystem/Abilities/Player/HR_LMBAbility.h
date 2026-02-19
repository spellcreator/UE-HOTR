// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HR_GameplayAbility.h"
#include "HR_LMBAbility.generated.h"

/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_LMBAbility : public UHR_GameplayAbility
{
	GENERATED_BODY()
	
public:

	
	UFUNCTION(BlueprintCallable, Category = "Crash|Abilities")
	void SendHitReactEventsToActors(const TArray<AActor*>& ActorsHit);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Abilities")
	float HitBoxRadius = 100.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Abilities")
	float HitBoxForwardUpset = 200.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Abilities")
	float HitboxElevationOffset = 20.0f;
};
