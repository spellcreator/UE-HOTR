// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HR_GameplayAbility.h"
#include "HR_Ability_Attack_JumpAttack.generated.h"


class AHR_BaseCharacter;
/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_Ability_Attack_JumpAttack : public UHR_GameplayAbility
{
	GENERATED_BODY()
public:
	UHR_Ability_Attack_JumpAttack();
	
	virtual void ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData) override;
	void OnJumpCompleted();
	void OnJumpInterrupted();

	void LaunchChar();
	void FinishAbility();
	
private:
	
	UPROPERTY(EditDefaultsOnly)
	float FlightDuration = 0.5f;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0", ClampMax = "1000"))
	float ArcStrength = 0.5f;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveFloat> JumpHeightCurve;
};
