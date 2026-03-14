// 


#include "Utils/HR_BlueprintLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/HR_AttributeSet.h"
#include "AbilitySystem/Core/Enums/HR_HitboxShape.h"
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


// ═════════════════════════════════════════════════════════════════════════════
// Unified HitboxOverlapTest
// ═════════════════════════════════════════════════════════════════════════════

TArray<AActor*> UHR_BlueprintLibrary::HitboxOverlapTest(
	AActor* AvatarActor,
	EHR_HitboxShape Shape,
	float Radius,
	float ForwardOffset,
	float ElevationOffset,
	const FVector& Direction,
	float ConeHalfAngleDeg,
	float ConeRange,
	const FVector& BoxHalfExtent,
	bool bDrawDebug)
{
	if (!IsValid(AvatarActor)) return {};

	UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return {};

	// ── Общие данные ─────────────────────────────────────────────────────
	const FVector ActorForward = AvatarActor->GetActorForwardVector().GetSafeNormal2D();
	const FVector DirNorm2D = Direction.IsNearlyZero() ? ActorForward : Direction.GetSafeNormal2D();
	const FVector ActorLoc = AvatarActor->GetActorLocation();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);

	TArray<FOverlapResult> OverlapResults;
	TArray<AActor*> ActorsHit;

	// ─────────────────────────────────────────────────────────────────────
	switch (Shape)
	{
	// ── SPHERE ────────────────────────────────────────────────────────────
	case EHR_HitboxShape::Sphere:
	{
		const FVector Forward = ActorForward * ForwardOffset;
		const FVector Center = ActorLoc + Forward + FVector(0, 0, ElevationOffset);

		World->OverlapMultiByChannel(
			OverlapResults, Center, FQuat::Identity,
			ECC_Visibility, FCollisionShape::MakeSphere(Radius),
			QueryParams, ResponseParams);

		for (const FOverlapResult& Result : OverlapResults)
		{
			AHR_BaseCharacter* Char = Cast<AHR_BaseCharacter>(Result.GetActor());
			if (!IsValid(Char) || !Char->IsAlive()) continue;
			ActorsHit.AddUnique(Char);
		}

		if (bDrawDebug)
		{
			DrawDebugSphereHitbox(AvatarActor, Center, Radius, OverlapResults);
		}
		break;
	}

	// ── CONE ──────────────────────────────────────────────────────────────
	case EHR_HitboxShape::Cone:
	{
		const FVector Origin = ActorLoc + FVector(0, 0, ElevationOffset);
		const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(ConeHalfAngleDeg));

		// Сферический оверлап по дальности конуса
		World->OverlapMultiByChannel(
			OverlapResults, Origin, FQuat::Identity,
			ECC_Visibility, FCollisionShape::MakeSphere(ConeRange),
			QueryParams, ResponseParams);

		// Фильтрация по углу
		for (const FOverlapResult& Result : OverlapResults)
		{
			AHR_BaseCharacter* Char = Cast<AHR_BaseCharacter>(Result.GetActor());
			if (!IsValid(Char) || !Char->IsAlive()) continue;

			const FVector ToTarget = (Char->GetActorLocation() - Origin).GetSafeNormal2D();
			const float Dot = FVector::DotProduct(DirNorm2D, ToTarget);

			if (Dot >= CosHalfAngle)
			{
				ActorsHit.AddUnique(Char);
			}
		}

		if (bDrawDebug)
		{
			DrawDebugConeHitbox(AvatarActor, Origin, DirNorm2D, ConeRange, ConeHalfAngleDeg, ActorsHit);
		}
		break;
	}

	// ── BOX ───────────────────────────────────────────────────────────────
	case EHR_HitboxShape::Box:
	{
		// Бокс ориентирован по Direction, смещён на ForwardOffset
		const FVector Forward = DirNorm2D * ForwardOffset;
		const FVector Center = ActorLoc + Forward + FVector(0, 0, ElevationOffset);

		// Ротация бокса по направлению
		const FQuat BoxRotation = DirNorm2D.ToOrientationQuat();

		World->OverlapMultiByChannel(
			OverlapResults, Center, BoxRotation,
			ECC_Visibility, FCollisionShape::MakeBox(BoxHalfExtent),
			QueryParams, ResponseParams);

		for (const FOverlapResult& Result : OverlapResults)
		{
			AHR_BaseCharacter* Char = Cast<AHR_BaseCharacter>(Result.GetActor());
			if (!IsValid(Char) || !Char->IsAlive()) continue;
			ActorsHit.AddUnique(Char);
		}

		if (bDrawDebug)
		{
			DrawDebugBoxHitbox(AvatarActor, Center, BoxHalfExtent, BoxRotation, ActorsHit);
		}
		break;
	}

	default:
		break;
	}

	return ActorsHit;
}


// ═════════════════════════════════════════════════════════════════════════════
// Debug Drawing
// ═════════════════════════════════════════════════════════════════════════════

void UHR_BlueprintLibrary::DrawDebugSphereHitbox(
	const UObject* WorldContext,
	const FVector& Center, float Radius,
	const TArray<FOverlapResult>& Overlaps)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return;

	DrawDebugSphere(World, Center, Radius, 25, FColor::Red, false, 4.f);

	for (const FOverlapResult& Result : Overlaps)
	{
		if (IsValid(Result.GetActor()))
		{
			FVector Loc = Result.GetActor()->GetActorLocation();
			Loc.Z += 100.f;
			DrawDebugSphere(World, Loc, 30.f, 10, FColor::Green, false, 3.f);
		}
	}
}

void UHR_BlueprintLibrary::DrawDebugConeHitbox(
	const UObject* WorldContext,
	const FVector& Origin, const FVector& Dir, float Range, float HalfAngleDeg,
	const TArray<AActor*>& HitActors)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return;

	// Центральная линия
	DrawDebugLine(World, Origin, Origin + Dir * Range, FColor::Yellow, false, 4.f, 0, 2.f);

	// Левая и правая граница
	const FVector Right = Dir.RotateAngleAxis(HalfAngleDeg, FVector::UpVector);
	const FVector Left = Dir.RotateAngleAxis(-HalfAngleDeg, FVector::UpVector);
	DrawDebugLine(World, Origin, Origin + Right * Range, FColor::Orange, false, 4.f, 0, 2.f);
	DrawDebugLine(World, Origin, Origin + Left * Range, FColor::Orange, false, 4.f, 0, 2.f);

	// Дуга
	constexpr int32 Segments = 12;
	const float AngleStep = (HalfAngleDeg * 2.f) / Segments;
	for (int32 i = 0; i < Segments; ++i)
	{
		const float A1 = -HalfAngleDeg + AngleStep * i;
		const float A2 = -HalfAngleDeg + AngleStep * (i + 1);
		const FVector P1 = Origin + Dir.RotateAngleAxis(A1, FVector::UpVector) * Range;
		const FVector P2 = Origin + Dir.RotateAngleAxis(A2, FVector::UpVector) * Range;
		DrawDebugLine(World, P1, P2, FColor::Orange, false, 4.f, 0, 2.f);
	}

	// Попавшие акторы
	for (AActor* Hit : HitActors)
	{
		if (!IsValid(Hit)) continue;
		FVector Loc = Hit->GetActorLocation();
		Loc.Z += 100.f;
		DrawDebugSphere(World, Loc, 30.f, 10, FColor::Green, false, 3.f);
	}
}

void UHR_BlueprintLibrary::DrawDebugBoxHitbox(
	const UObject* WorldContext,
	const FVector& Center, const FVector& HalfExtent, const FQuat& Rotation,
	const TArray<AActor*>& HitActors)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return;

	DrawDebugBox(World, Center, HalfExtent, Rotation, FColor::Red, false, 4.f, 0, 2.f);

	for (AActor* Hit : HitActors)
	{
		if (!IsValid(Hit)) continue;
		FVector Loc = Hit->GetActorLocation();
		Loc.Z += 100.f;
		DrawDebugSphere(World, Loc, 30.f, 10, FColor::Green, false, 3.f);
	}
}
