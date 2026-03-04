// 


#include "Player/HR_UnitTargetingComponent.h"

#include "AbilitySystem/HR_AttributeSet.h"
#include "Characters/HR_BaseCharacter.h"
#include "Components/DecalComponent.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "TargetSystem/Interface/HR_Targetable.h"

UHR_UnitTargetingComponent::UHR_UnitTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UHR_UnitTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!HasTarget()) return;
	if (bAutoClearInvalidTarget && !IsValidTarget(CurrentTarget))
	{
		ClearTarget();
		return;
	}
	UpdateTargetOfTarget();
}

AActor* UHR_UnitTargetingComponent::TryTargetUnderCursor()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow,
	FString::Printf(TEXT("TryTarget on: %s"), *GetName()));
	APlayerController* PC = GetPlayerController();
	if (!PC) return nullptr;
	FHitResult HitResult;
	if (!PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		ClearTarget();
		return nullptr;
	}
	AActor* HitActor = HitResult.GetActor();
	if (HitActor && HitActor == PC->GetPawn()) return CurrentTarget;
	if (!HitActor || !IsValidTarget(HitActor))
	{
		ClearTarget();
		return nullptr;
	}
	if (HitActor == CurrentTarget)
	{
		return nullptr;
	}
	SetTarget(HitActor);
	return HitActor;
}

void UHR_UnitTargetingComponent::SetTarget(AActor* NewTarget)
{
	if (NewTarget == CurrentTarget) return;

	AActor* OldTarget = CurrentTarget;
	CurrentTarget = NewTarget;
	CachedTargetOfTarget = nullptr;
	CycleIndex = 0;

	UpdateVisualization(OldTarget);

	OnTargetChanged.Broadcast(CurrentTarget);
	if (HasTarget())
	{
		UpdateTargetOfTarget();
	}
	else
	{
		OnTargetOfTargetChanged.Broadcast(nullptr);
	}
}

void UHR_UnitTargetingComponent::ClearTarget()
{
	SetTarget(nullptr);
}

AActor* UHR_UnitTargetingComponent::CycleTarget(float SearchRadius)
{
	APlayerController* PC = GetPlayerController();
	if (!PC || !PC->GetPawn()) return nullptr;
	const FVector Origin = PC->GetPawn()->GetActorLocation();
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PC->GetPawn());
	GetWorld()->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity,
		TraceChannel, FCollisionShape::MakeSphere(SearchRadius), QueryParams);
	TArray<AActor*> Candidates;
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Actor = Overlap.GetActor();
		if (Actor && IsValidTarget(Actor) && !Candidates.Contains(Actor))
			Candidates.Add(Actor);
	}
	if (Candidates.Num() == 0) return nullptr;
	Candidates.Sort([&Origin](const AActor& A, const AActor& B)
	{
		return FVector::DistSquared(A.GetActorLocation(), Origin)
			< FVector::DistSquared(B.GetActorLocation(), Origin);
	});
	CycleIndex = CycleIndex % Candidates.Num();
	AActor* Target = Candidates[CycleIndex];

	// Skip current target — jump to next
	if (Target == CurrentTarget && Candidates.Num() > 1)
	{
		CycleIndex = (CycleIndex + 1) % Candidates.Num();
		Target = Candidates[CycleIndex];
	}

	int32 NextIndex = CycleIndex + 1;
	SetTarget(Target);
	CycleIndex = NextIndex;
	return Target;
}

FHR_TargetInfo UHR_UnitTargetingComponent::GetTargetInfo() const
{
	return BuildTargetInfo(CurrentTarget);
}

FHR_TargetInfo UHR_UnitTargetingComponent::GetTargetOfTargetInfo() const
{
	if (!HasTarget()) return FHR_TargetInfo();
	return BuildTargetInfo(CachedTargetOfTarget);
}

APlayerController* UHR_UnitTargetingComponent::GetPlayerController() const
{
	if (AActor* Owner = GetOwner())
	{
		if (APlayerController* PC = Cast<APlayerController>(Owner))
			return PC;
		if (APawn* Pawn = Cast<APawn>(Owner))
			return Cast<APlayerController>(Pawn->GetController());
	}
	return nullptr;
}

FHR_TargetInfo UHR_UnitTargetingComponent::BuildTargetInfo(AActor* Actor) const
{
	FHR_TargetInfo Info;
	if (!IsValid(Actor)) return Info;
	IHR_Targetable* Targetable = Cast<IHR_Targetable>(Actor);
	if (!Targetable) return Info;
	Info.TargetActor = Actor;
	Info.DisplayName = IHR_Targetable::Execute_GetTargetDisplayName(Actor);
	UAbilitySystemComponent* ASC = IHR_Targetable::Execute_GetTargetASC(Actor);
	if (ASC)
	{
		const UHR_AttributeSet* Attributes = ASC->GetSet<UHR_AttributeSet>();
		if (Attributes)
		{
			Info.Health    = Attributes->GetHealth();
			Info.MaxHealth = Attributes->GetMaxHealth();
			Info.Mana      = Attributes->GetMana();
			Info.MaxMana   = Attributes->GetMaxMana();
		}
	}
	Info.TargetOfTarget = CachedTargetOfTarget;
	return Info;
}

bool UHR_UnitTargetingComponent::IsValidTarget(AActor* Actor) const
{
	if (!IsValid(Actor)) return false;
	IHR_Targetable* Targetable = Cast<IHR_Targetable>(Actor);
	if (!Targetable) return false;
	return IHR_Targetable::Execute_CanBeTargeted(Actor);
}

void UHR_UnitTargetingComponent::UpdateTargetOfTarget()
{
	if (!HasTarget()) return;
	AActor* NewToT = nullptr;
	UHR_UnitTargetingComponent* TargetUTC = CurrentTarget->FindComponentByClass<UHR_UnitTargetingComponent>();
	if (TargetUTC && TargetUTC->HasTarget())
	{
		NewToT = TargetUTC->GetCurrentTarget();
	}
	if (NewToT != CachedTargetOfTarget)
	{
		CachedTargetOfTarget = NewToT;
		OnTargetOfTargetChanged.Broadcast(CachedTargetOfTarget);
	}
}

void UHR_UnitTargetingComponent::UpdateVisualization(AActor* OldTarget)
{
	// Hide decal on previous target
	if (IsValid(OldTarget))
	{
		AHR_BaseCharacter* OldChar = Cast<AHR_BaseCharacter>(OldTarget);
		if (OldChar && OldChar->SelectionDecalComponent)
		{
			OldChar->SelectionDecalComponent->SetHiddenInGame(true);
		}
	}

	// Show decal on new target
	if (HasTarget())
	{
		AHR_BaseCharacter* NewChar = Cast<AHR_BaseCharacter>(CurrentTarget);
		if (NewChar && NewChar->SelectionDecalComponent)
		{
			if (TargetDecalMaterial)
			{
				NewChar->SelectionDecalComponent->SetDecalMaterial(TargetDecalMaterial);
			}
			NewChar->SelectionDecalComponent->SetHiddenInGame(false);
		}
	}
}