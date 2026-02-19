// 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HR_BlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UHR_BlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void SendDamageEventToPlayer(AActor* Target, 
		const TSubclassOf<UGameplayEffect>& DamageEffect,UPARAM(ref) FGameplayEventData& Payload, const FGameplayTag& DataTag,
		float Damage, const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem = nullptr);
	
	UFUNCTION(BlueprintCallable)
	static void SendDamageEventToPlayers(TArray<AActor*> Targets, 
		const TSubclassOf<UGameplayEffect>& DamageEffect,UPARAM(ref) FGameplayEventData& Payload, const FGameplayTag& DataTag,
		float Damage, const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem = nullptr);
	
	
	UFUNCTION(BlueprintCallable, Category = "Crash|Abilities")
	static TArray<AActor*> HitboxOverlapTest(AActor* AvatarActor, float HitBoxRadius, float HitBoxForwardUpset = 0.f, float HitboxElevationOffset = 0.f, bool bDrawDebug = false);
	
	
	static void DrawHitBoxOverlapResults(const UObject* WorldContext,const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation, float HitBoxRadius); 
	
};
