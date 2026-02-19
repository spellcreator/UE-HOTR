// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroesOfTheRearguard/Public/Characters/HR_PlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/HR_AttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/HR_PlayerState.h"


class AHR_PlayerState;
// Sets default values
AHR_PlayerCharacter::AHR_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f,96.f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = true;
	bUseControllerRotationYaw = true;
	
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f,540.f,0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArmComp"));
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->TargetArmLength = 500.0f;
	SpringArmComp->bUsePawnControlRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	CameraComponent->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
}


UAbilitySystemComponent* AHR_PlayerCharacter::GetAbilitySystemComponent() const
{
	AHR_PlayerState* HRPlayerState = Cast<AHR_PlayerState>(GetPlayerState());
	if (!IsValid(HRPlayerState)) return nullptr;
	
	return HRPlayerState->GetAbilitySystemComponent();
}

UAttributeSet* AHR_PlayerCharacter::GetAttributeSet() const
{
	AHR_PlayerState* HRPlayerState = Cast<AHR_PlayerState>(GetPlayerState());
	if (!IsValid(HRPlayerState)) return nullptr;
	
	return HRPlayerState->GetAttributeSet();
}

void AHR_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (!IsValid(GetAbilitySystemComponent()) || !HasAuthority()) return;
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());
	GiveStartupAbilities();
	InitializeAttributes();
	
	UHR_AttributeSet* HR_AttributeSet = Cast<UHR_AttributeSet>(GetAttributeSet());
	if (!IsValid(HR_AttributeSet)) return;
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(HR_AttributeSet->GetHealthAttribute()).AddUObject(this,&ThisClass::OnHealthChange);
}

void AHR_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (!IsValid(GetAbilitySystemComponent())) return;
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(),this);
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());
	
	UHR_AttributeSet* HR_AttributeSet = Cast<UHR_AttributeSet>(GetAttributeSet());
	if (!IsValid(HR_AttributeSet)) return;
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(HR_AttributeSet->GetHealthAttribute()).AddUObject(this,&ThisClass::OnHealthChange);
}
