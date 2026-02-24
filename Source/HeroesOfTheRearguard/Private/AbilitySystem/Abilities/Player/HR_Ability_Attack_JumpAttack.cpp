// 


#include "AbilitySystem/Abilities/Player/HR_Ability_Attack_JumpAttack.h"

#include "Characters/HR_BaseCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTags/HRTags.h"
#include "Kismet/GameplayStatics.h"
#include "Tasks/AbilityTask_JumpToLocation.h"

UHR_Ability_Attack_JumpAttack::UHR_Ability_Attack_JumpAttack()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = HRTags::HRAbilities::JumpAttack;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;

	AbilityTriggers.Add(TriggerData);
}

void UHR_Ability_Attack_JumpAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	FVector TargetLocation = FVector::ZeroVector;

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		const FGameplayAbilityTargetData* Data = TriggerEventData->TargetData.Get(0);
		if (Data && Data->HasEndPoint())
		{
			TargetLocation = Data->GetEndPoint();

			AHR_BaseCharacter* PlayerCharacter = Cast<AHR_BaseCharacter>(ActorInfo->AvatarActor.Get());
			if (!IsValid(PlayerCharacter)) { EndAbility(Handle, ActorInfo, ActivationInfo, true, true); return; }

			UAbilityTask_JumpToLocation* JumpTask = UAbilityTask_JumpToLocation::JumpToLocation(
				this,
				TargetLocation,
				FlightDuration,          // Duration
				ArcStrength,         // ArcHeight
				JumpHeightCurve,  // UCurveFloat* — назначаешь в Blueprint/редакторе
				nullptr
				);

			// Привязываем завершение к EndAbility
			JumpTask->OnCompleted.AddDynamic(this, &UHR_Ability_Attack_JumpAttack::OnJumpCompleted);
			JumpTask->OnInterrupted.AddDynamic(this, &UHR_Ability_Attack_JumpAttack::OnJumpInterrupted);
			JumpTask->ReadyForActivation();
		}
	}
}

void UHR_Ability_Attack_JumpAttack::OnJumpCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UHR_Ability_Attack_JumpAttack::OnJumpInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

