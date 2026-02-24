// 


#include "AbilitySystem/Abilities/Player/HR_Ability_Attack_JumpAttack.h"

#include "Characters/HR_BaseCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTags/HRTags.h"
#include "Kismet/GameplayStatics.h"

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
			LaunchChar(PlayerCharacter, TargetLocation, 150.f);
		}
	}
}

void UHR_Ability_Attack_JumpAttack::LaunchChar(AHR_BaseCharacter* PCharacter, FVector& EndPoint, float ArcHeight)
{

	FVector Start = PCharacter->GetActorLocation();
	FVector LaunchVelocity;

	bool bFound = UGameplayStatics::SuggestProjectileVelocity(
		this,
		LaunchVelocity,
		Start,
		EndPoint,
		ArcHeight,               // Override gravity Z (можем регулировать дугу)
		0.f,
		0.f,
		ESuggestProjVelocityTraceOption::DoNotTrace
	);

	if (bFound)
	{
		PCharacter->LaunchCharacter(LaunchVelocity, true, true);

		DrawDebugTrajectory(PCharacter,Start, LaunchVelocity);
	}
}

void UHR_Ability_Attack_JumpAttack::FinishAbility()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UHR_Ability_Attack_JumpAttack::DrawDebugTrajectory(AHR_BaseCharacter* PCharacter,FVector Start, FVector Velocity)
{
	FPredictProjectilePathParams Params;
	Params.StartLocation = Start;
	Params.LaunchVelocity = Velocity;
	Params.ProjectileRadius = 5.f;
	Params.MaxSimTime = 2.f;
	Params.bTraceWithCollision = false;
	Params.SimFrequency = 15.f;
	Params.OverrideGravityZ = PCharacter->GetCharacterMovement()->GetGravityZ();

	FPredictProjectilePathResult Result;

	UGameplayStatics::PredictProjectilePath(this, Params, Result);

	for (const FPredictProjectilePathPointData& Point : Result.PathData)
	{
		DrawDebugSphere(GetWorld(), Point.Location, 6.f, 8, FColor::Green, false, 3.f);
	}
}
