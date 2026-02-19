// 


#include "AbilitySystem/Abilities/Player/ChargeAbility.h"

#include "Characters/HR_PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitMovementModeChange.h"
#include "GameFramework/CharacterMovementComponent.h"

UChargeAbility::UChargeAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UChargeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AHR_PlayerCharacter* Character = Cast<AHR_PlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Character) 
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	FVector Direction = GetChargeDirection(Character);
	
	ApplyChargeImpulse(Character, Direction);
}

FVector UChargeAbility::GetChargeDirection(ACharacter* PlayerCharacter) const
{
	if (!PlayerCharacter) return FVector::ZeroVector;

	FVector Velocity = PlayerCharacter->GetVelocity();
	Velocity.Z = 0.f;
	
	FVector MoveDirection;
	if (!Velocity.IsNearlyZero())
	{
		return MoveDirection = Velocity.GetSafeNormal();
	}
	return MoveDirection = PlayerCharacter->GetActorForwardVector();
}

void UChargeAbility::ApplyChargeImpulse(ACharacter* PlayerCharacter, const FVector& Direction)
{
	if (!PlayerCharacter) return;
	FVector LaunchVelocity = Direction * ChargeStrength;
	LaunchVelocity.Z = ChargeZVelocity;
	
	PlayerCharacter->LaunchCharacter(LaunchVelocity, true, true);
}


void UChargeAbility::OnCharacterLanded(const FHitResult& Hit)
{
    
	AHR_PlayerCharacter* Character = Cast<AHR_PlayerCharacter>(GetAvatarActorFromActorInfo());
	if (Character)
	{
		Character->LandedDelegate.RemoveDynamic(this, &ThisClass::OnCharacterLanded);
	}
    
	FinishCharge();
}

void UChargeAbility::FinishCharge()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UChargeAbility::AddCollisionResponse(ACharacter* PlayerCharacter, const ECollisionResponse CollisionResponse) const
{
	UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent();
	if (!Capsule) return;
	Capsule->SetCollisionResponseToChannel(ECC_Pawn, CollisionResponse);
	Capsule->SetCollisionResponseToChannel(ECC_Visibility, CollisionResponse);
	Capsule->SetCollisionResponseToChannel(ECC_Camera, CollisionResponse);
	
}

