// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroesOfTheRearguard/Public/Characters/HR_BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/DecalComponent.h"
#include "Net/UnrealNetwork.h"

AHR_BaseCharacter::AHR_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->bReceivesDecals = false;
	
	SelectionDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal"));
	SelectionDecalComponent->SetupAttachment(GetRootComponent());
	SelectionDecalComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	SelectionDecalComponent->DecalSize = FVector(200.f, 128.f, 128.f);
	SelectionDecalComponent->SetHiddenInGame(true);   // <-- hidden at start, NOT SetVisibility
	SelectionDecalComponent->SetVisibility(true);       // <-- keep editor-visible so you can see it in viewport
	SelectionDecalComponent->bDestroyOwnerAfterFade = false;
}

void AHR_BaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bAlive);
	DOREPLIFETIME(ThisClass, bCharged);
}

UAbilitySystemComponent* AHR_BaseCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}

void AHR_BaseCharacter::GiveStartupAbilities()
{
	if (!IsValid(GetAbilitySystemComponent())) return;
	for (const auto& Ability : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
	}
}

void AHR_BaseCharacter::InitializeAttributes() const
{
	checkf(IsValid(InitializeAttributesEffect), TEXT("InitializeAttributeEffects not set"))
	
	FGameplayEffectContextHandle ContextHandle =  GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle =  GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
}

void AHR_BaseCharacter::OnHealthChange(const FOnAttributeChangeData& AttributeChangeData)
{
	if (AttributeChangeData.NewValue <= 0.f) 
	{
		HandleDeath();
	}
}

void AHR_BaseCharacter::HandleDeath()
{
	bAlive = false;
}

void AHR_BaseCharacter::HandleRespawn()
{
	bAlive = true;
}

void AHR_BaseCharacter::ResetAttributes()
{
	/*checkf(IsValid(ResetAttributesEffect), TEXT("ResetAttributesEffect not set"))
	if (IsValid(GetAbilitySystemComponent())) return;
	
	FGameplayEffectContextHandle ContextHandle =  GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle =  GetAbilitySystemComponent()->MakeOutgoingSpec(ResetAttributesEffect, 1.f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());*/
}

// ---- IHR_Targetable Implementation ----------------------------------------

bool AHR_BaseCharacter::CanBeTargeted_Implementation() const
{
	return IsAlive();
}

FText AHR_BaseCharacter::GetTargetDisplayName_Implementation() const
{
	return TargetDisplayName;
}

UAbilitySystemComponent* AHR_BaseCharacter::GetTargetASC_Implementation() const
{
	return GetAbilitySystemComponent();
}

FVector AHR_BaseCharacter::GetTargetIndicatorLocation_Implementation() const
{
	// Above the capsule top
	return GetActorLocation() + FVector(0.f, 0.f, 120.f);
}

