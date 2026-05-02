// 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HR_BlueprintLibrary.generated.h"

struct FGameplayTag;
class UGameplayEffect;
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
	
	/**
	 * Универсальный хитбокс-тест с поддержкой Sphere / Cone / Box.
	 *
	 * @param AvatarActor       Актор-источник (его позиция = центр)
	 * @param Shape             Форма хитбокса
	 * @param Radius            Радиус сферы (Sphere) — игнорируется для Cone/Box
	 * @param ForwardOffset     Смещение центра вперёд от актора
	 * @param ElevationOffset   Смещение центра по Z
	 * @param Direction         Направление (для Cone/Box). ZeroVector = ActorForward
	 * @param ConeHalfAngleDeg  Половина угла конуса (только Cone)
	 * @param ConeRange         Дальность конуса (только Cone)
	 * @param BoxHalfExtent     Полуразмеры бокса (только Box)
	 * @param bDrawDebug        Рисовать debug-визуализацию
	 */
	UFUNCTION(BlueprintCallable, Category = "Crash|Abilities")
	static TArray<AActor*> HitboxOverlapTest(
		AActor* AvatarActor,
		EHR_HitboxShape Shape,
		float Radius = 150.f,
		float ForwardOffset = 0.f,
		float ElevationOffset = 0.f,
		const FVector& Direction = FVector::ZeroVector,
		float ConeHalfAngleDeg = 45.f,
		float ConeRange = 600.f,
		const FVector& BoxHalfExtent = FVector(100.f),
		bool bDrawDebug = false);
	
	// ─── Debug drawing ───────────────────────────────────────────────────
	
	static void DrawDebugSphereHitbox(const UObject* WorldContext,
		const FVector& Center, float Radius,
		const TArray<FOverlapResult>& Overlaps);
	
	static void DrawDebugConeHitbox(const UObject* WorldContext,
		const FVector& Origin, const FVector& Dir, float Range, float HalfAngleDeg,
		const TArray<AActor*>& HitActors);
	
	static void DrawDebugBoxHitbox(const UObject* WorldContext,
		const FVector& Center, const FVector& HalfExtent, const FQuat& Rotation,
		const TArray<AActor*>& HitActors);
};