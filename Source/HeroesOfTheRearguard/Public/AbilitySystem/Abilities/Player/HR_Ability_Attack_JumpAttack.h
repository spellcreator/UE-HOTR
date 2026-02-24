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
	
	void LaunchChar(AHR_BaseCharacter* PCharacter, FVector& Direction, float ArcHeight);
	void FinishAbility();
	void DrawDebugTrajectory(AHR_BaseCharacter* PCharacter,FVector Start, FVector Velocity);
};
