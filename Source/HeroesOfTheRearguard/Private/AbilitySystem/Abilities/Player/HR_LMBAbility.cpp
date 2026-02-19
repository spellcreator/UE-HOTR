// 


#include "AbilitySystem/Abilities/Player/HR_LMBAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GamePlayTags/HRTags.h"


void UHR_LMBAbility::SendHitReactEventsToActors(const TArray<AActor*>& ActorsHit)
{
	for (AActor* HitActor : ActorsHit)
	{
		
		FGameplayEventData PayLoad;
		PayLoad.Instigator = GetAvatarActorFromActorInfo();
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, HRTags::Events::Enemy::HitReact,PayLoad );
	}
}
