// 


#include "AbilitySystem/HR_AbilitySystemComponent.h"

#include "AbilitySystem/Core/HR_GameplayAbility.h"
#include "GameplayTags/HRTags.h"


void UHR_AbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	
	HandleAutoActivateAbilities(AbilitySpec);
}

void UHR_AbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
    
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.IsActive())
		{
			HandleAutoActivateAbilities(AbilitySpec);
		}
	}
}

void UHR_AbilitySystemComponent::SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
	if (IsValid(GetAvatarActor()) && !GetAvatarActor()->HasAuthority()) return;
	
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass);
	
	if (AbilitySpec)
	{
		AbilitySpec->Level = Level;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UHR_AbilitySystemComponent::AddAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
	if (IsValid(GetAvatarActor()) && !GetAvatarActor()->HasAuthority()) return;
	
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass);
	
	if (AbilitySpec)
	{
		AbilitySpec->Level += Level;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

bool UHR_AbilitySystemComponent::TryActivateAbilityByTag(const FGameplayTag& Tag)
{
	FGameplayTagContainer Container(Tag);
	return TryActivateAbilitiesByTag(Container);
}


UHR_GameplayAbility* UHR_AbilitySystemComponent::FindAbilityByTag(const FGameplayTag& Tag) const
{
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (!IsValid(Spec.Ability)) continue;
		if (Spec.Ability->GetAssetTags().HasTagExact(Tag))
			return Cast<UHR_GameplayAbility>(Spec.Ability);
	}
	return nullptr;
}

void UHR_AbilitySystemComponent::HandleAutoActivateAbilities(const FGameplayAbilitySpec& AbilitySpec)
{
	if (!IsValid(AbilitySpec.Ability)) return;
	
	for (const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())
	{
		if (Tag.MatchesTagExact(HRTags::HRAbilities::ActivateOnGiven))
		{
			TryActivateAbility(AbilitySpec.Handle);
			return;
		}
	}
}

bool UHR_AbilitySystemComponent::RequiresTargeting(const FGameplayTag& Tag) const
{
	if (UHR_GameplayAbility* Ability = FindAbilityByTag(Tag))
	{
		return Ability->RequiresTargeting();
	}
	return false;
}
