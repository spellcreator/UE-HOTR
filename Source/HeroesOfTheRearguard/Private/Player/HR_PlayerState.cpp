// 


#include "Player/HR_PlayerState.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/HR_AbilitySystemComponent.h"
#include "AbilitySystem/HR_AttributeSet.h"

AHR_PlayerState::AHR_PlayerState()
{
	SetNetUpdateFrequency(100.f);
	
	AbilitySystemComponent = CreateDefaultSubobject<UHR_AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UHR_AttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AHR_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
