// 


#include "Utils/HR_BlueprintLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/HR_AttributeSet.h"
#include "Characters/HR_BaseCharacter.h"
#include "Engine/OverlapResult.h"
#include "GamePlayTags/HRTags.h"


void UHR_BlueprintLibrary::SendDamageEventToPlayer(AActor* Target, const TSubclassOf<UGameplayEffect>& DamageEffect,
                                                   FGameplayEventData& Payload, const FGameplayTag& DataTag, float Damage, const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem)
{
	AHR_BaseCharacter* PlayerCharacter = Cast<AHR_BaseCharacter>(Target);
	if(!IsValid(PlayerCharacter)) return;
	if (!PlayerCharacter->IsAlive()) return;
	
	FGameplayTag EventTag;
	if (!EventTagOverride.MatchesTagExact(HRTags::None))
	{
		EventTag = EventTagOverride;
	}
	else
	{
		UHR_AttributeSet* AttributeSet = Cast<UHR_AttributeSet>(PlayerCharacter->GetAttributeSet());
		if (!IsValid(AttributeSet)) return;
	
		const bool bLethal = AttributeSet->GetHealth() - Damage <= 0.0f;
		//EventTag = bLethal ? HRTags::::Events::Player::Death : HRTags::::Events::Player::HitReact;
	}
	
	
	
	Payload.OptionalObject = OptionalParticleSystem;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter, EventTag, Payload);
	
	UAbilitySystemComponent* TargetASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(TargetASC)) return;
	
	FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffect, 1.f, ContextHandle);
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,DataTag, -Damage);
	
	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UHR_BlueprintLibrary::SendDamageEventToPlayers(TArray<AActor*> Targets,
	const TSubclassOf<UGameplayEffect>& DamageEffect, FGameplayEventData& Payload, const FGameplayTag& DataTag,
	float Damage, const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem)
{
	for (AActor* Target : Targets)
	{
		SendDamageEventToPlayer(Target,DamageEffect,Payload,DataTag,Damage,EventTagOverride,OptionalParticleSystem);
	}
}

TArray<AActor*> UHR_BlueprintLibrary::HitboxOverlapTest(AActor* AvatarActor, float HitBoxRadius, float HitBoxForwardUpset, float HitboxElevationOffset, bool bDrawDebug)
{
	if (!IsValid(AvatarActor)) return TArray<AActor*>();
	// add if actors more than one
	// TArray<AActor*> ActorsToIgnore;
	// ActorsToIgnore.Add(AvatarActor);
	// QueryParams.AddIgnoredActors(ActorsToIgnore);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	
	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);
	
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitBoxRadius);
	
	const FVector Forward = AvatarActor->GetActorForwardVector() * HitBoxForwardUpset;
	const FVector HitBoxLocation = AvatarActor->GetActorLocation() + Forward + FVector(0,0,HitboxElevationOffset);
	
	UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return TArray<AActor*>();
	World->OverlapMultiByChannel(OverlapResults, HitBoxLocation, FQuat::Identity, ECC_Visibility, Sphere, QueryParams, ResponseParams );
	
	TArray<AActor*> ActorsHit;
	for (const FOverlapResult& Result : OverlapResults)
	{
		
		AHR_BaseCharacter* BaseCharacter = Cast<AHR_BaseCharacter>(Result.GetActor());
		if (!IsValid(BaseCharacter)) continue;
		if (!BaseCharacter->IsAlive()) continue;
		ActorsHit.AddUnique(BaseCharacter);
	}
	
	
	if (bDrawDebug)
	{
		DrawHitBoxOverlapResults(AvatarActor,OverlapResults,HitBoxLocation, HitBoxRadius);
	}
	
	return ActorsHit;
}

void UHR_BlueprintLibrary::DrawHitBoxOverlapResults(const UObject* WorldContext,const TArray<FOverlapResult>& OverlapResults,
	const FVector& HitBoxLocation,float HitBoxRadius)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return;
	DrawDebugSphere(World,HitBoxLocation,HitBoxRadius,25,FColor::Red,false,4.0f);
		
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (IsValid(Result.GetActor()))
		{
			FVector DebugLocation = Result.GetActor()->GetActorLocation();
			DebugLocation.Z += 100.f;
			DrawDebugSphere(World,DebugLocation,30.f,10,FColor::Green,false,3.0f);
		}
	}
}
