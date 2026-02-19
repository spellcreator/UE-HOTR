// 


#include "Characters/HR_MoneyKenn.h"

#include "AbilitySystem/HR_AbilitySystemComponent.h"
#include "AbilitySystem/HR_AttributeSet.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AHR_MoneyKenn::AHR_MoneyKenn()
{
	PrimaryActorTick.bCanEverTick = false;
	
	AbilitySystemComponent = CreateDefaultSubobject<UHR_AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UHR_AttributeSet>("AttributeSet");
}

void AHR_MoneyKenn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

UAbilitySystemComponent* AHR_MoneyKenn::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AHR_MoneyKenn::GetAttributeSet() const
{
	return AttributeSet;
}

void AHR_MoneyKenn::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(GetAbilitySystemComponent())) return;
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(this,this);
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());
	
	if (!HasAuthority()) return;
	
	GiveStartupAbilities();
	InitializeAttributes();
	
	UHR_AttributeSet* GC_AttributeSet = Cast<UHR_AttributeSet>(GetAttributeSet());
	if (!IsValid(GC_AttributeSet)) return;
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(GC_AttributeSet->GetHealthAttribute()).AddUObject(this,&ThisClass::OnHealthChange);
}


