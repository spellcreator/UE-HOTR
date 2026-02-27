#include "AbilitySystem/Abilities/Player/HR_Ability_Charge.h"

#include "Characters/HR_PlayerCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UChargeAbility::UChargeAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

// ─────────────────────────────────────────────────────────────────────────────

void UChargeAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    AHR_PlayerCharacter* Character = Cast<AHR_PlayerCharacter>(GetAvatarActorFromActorInfo());
    if (!Character)
    {
        EndAbilitySafe(true);
        return;
    }

    Character->LandedDelegate.AddDynamic(this, &ThisClass::OnLanded);

    if (ChargeMontage)
    {
        // Биндим делегаты до ReadyForActivation
        UAbilityTask_PlayMontageAndWait* MontageTask = PlayMontage(ChargeMontage);
        MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageEnded);
        MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageEnded);
        MontageTask->ReadyForActivation();
    }

    LaunchCharge(Character);
}

FVector UChargeAbility::GetChargeDirection(ACharacter* Character) const
{
    if (!Character) return FVector::ForwardVector;

    FVector Velocity = Character->GetVelocity();
    Velocity.Z = 0.f;

    return Velocity.IsNearlyZero()
        ? Character->GetActorForwardVector()
        : Velocity.GetSafeNormal();
}

void UChargeAbility::LaunchCharge(ACharacter* Character)
{
    FVector LaunchVelocity = GetChargeDirection(Character) * ChargeStrength;
    LaunchVelocity.Z = ChargeZVelocity;
    Character->LaunchCharacter(LaunchVelocity, true, true);
}

void UChargeAbility::OnLanded(const FHitResult& Hit)
{
    if (AHR_PlayerCharacter* Character = Cast<AHR_PlayerCharacter>(GetAvatarActorFromActorInfo()))
    {
        Character->LandedDelegate.RemoveDynamic(this, &ThisClass::OnLanded);
    }
    EndAbilitySafe(false);
}

void UChargeAbility::OnMontageEnded()
{
    if (AHR_PlayerCharacter* Character = Cast<AHR_PlayerCharacter>(GetAvatarActorFromActorInfo()))
    {
        Character->LandedDelegate.RemoveDynamic(this, &ThisClass::OnLanded);
    }
    EndAbilitySafe(true);
}