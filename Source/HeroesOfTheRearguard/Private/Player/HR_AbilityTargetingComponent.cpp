
#include "Player/HR_AbilityTargetingComponent.h"
#include "AbilitySystem/Core/HR_GameplayAbility.h"
#include "Components/DecalComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HR_PlayerController.h"

UHR_AbilityTargetingComponent::UHR_AbilityTargetingComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UHR_AbilityTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsTargeting) return;

    switch (CurrentTargetingType)
    {
    case EHR_AbilityTargetingType::GroundTarget:
        UpdateGroundTargetLocation();
        break;
    case EHR_AbilityTargetingType::DirectionalArc:
        UpdateDirectionalArc();
        break;
    default: break;
    }
}

void UHR_AbilityTargetingComponent::BeginTargeting(const FGameplayTag& AbilityTag, UHR_GameplayAbility* Ability)
{
    if (!IsValid(Ability)) return;

    bIsTargeting = true;
    PendingAbilityTag = AbilityTag;
    
    CurrentTargetingType = Ability->TargetingData.TargetingType;
    CurrentTargetingRadius = Ability->TargetingData.AOERadius;
    CurrentTargetingMaxCastRange = Ability->TargetingData.AbilityMaxRange;
    CurrentTargetingMinCastRange = Ability->TargetingData.AbilityMinRange;
    CurrentConeHalfAngle = Ability->TargetingData.ConeHalfAngleDeg;
    
    // Применяем визуал
    UMaterialInterface* Mat = IsValid(Ability->TargetingData.DecalMaterial)
        ? Ability->TargetingData.DecalMaterial
        : GroundTargetDecalMaterial;

    ShowDecal(CurrentTargetingRadius, CurrentTargetingType, Mat);
    UpdateDecalTransform(CurrentTargetLocation, CurrentTargetingRadius,CurrentTargetingRadius,  FRotator(-90.f, 0.f, 0.f));

    SetComponentTickEnabled(true);
}

void UHR_AbilityTargetingComponent::ConfirmTargeting()
{
    if (!bIsTargeting) return;

    
    const FVector ConfirmedLocation = CurrentTargetLocation;
    HideDecal();
    bIsTargeting = false;
    SetComponentTickEnabled(false);
        
    OnTargetingConfirmed.Broadcast(ConfirmedLocation);
}

void UHR_AbilityTargetingComponent::CancelTargeting()
{
    if (!bIsTargeting) return;

    HideDecal();
    bIsTargeting = false;
    SetComponentTickEnabled(false);
    PendingAbilityTag = FGameplayTag();
    
    OnTargetingCancelled.Broadcast();
}

void UHR_AbilityTargetingComponent::UpdateGroundTargetLocation()
{
    FVector HitLocation;
    if (!GetGroundLocationUnderCursor(HitLocation)) return;

    const AActor* Owner = GetOwner();
    if (!IsValid(Owner)) return;

    const FVector OwnerLoc = Owner->GetActorLocation();
    const FVector ToTarget = HitLocation - OwnerLoc;

    // Просто зажимаем по дальности, без смены цвета
    if (ToTarget.Size2D() > CurrentTargetingMaxCastRange)
    {
        const FVector ClampedXY = OwnerLoc + ToTarget.GetSafeNormal2D() * CurrentTargetingMaxCastRange;

        FHitResult RangeHit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Owner);

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            RangeHit,
            ClampedXY + FVector(0, 0, 300.f),
            ClampedXY - FVector(0, 0, 1000.f),
            ECC_WorldStatic,
            Params
        );

        HitLocation = bHit ? RangeHit.ImpactPoint + FVector(0, 0, 5.f) : ClampedXY;
    }
    else if (CurrentTargetingMinCastRange > 0 && ToTarget.Size2D() < CurrentTargetingMinCastRange)
    {
        const FVector ClampedXY = OwnerLoc + ToTarget.GetSafeNormal2D() * CurrentTargetingMinCastRange;

        FHitResult RangeHit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Owner);

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            RangeHit,
            ClampedXY + FVector(0, 0, 300.f),
            ClampedXY - FVector(0, 0, 1000.f),
            ECC_WorldStatic,
            Params
        );

        HitLocation = bHit ? RangeHit.ImpactPoint + FVector(0, 0, 5.f) : ClampedXY;
    }

    CurrentTargetLocation = HitLocation;
    UpdateDecalTransform(CurrentTargetLocation, CurrentTargetingRadius, CurrentTargetingRadius, FRotator(-90.f, 0.f, 0.f));
}

void UHR_AbilityTargetingComponent::UpdateDirectionalArc()
{
    FVector HitLocation;
    if (!GetGroundLocationUnderCursor(HitLocation)) return;

    const AActor* Owner = GetOwner();
    if (!IsValid(Owner)) return;

    const FVector OwnerLoc = Owner->GetActorLocation();
    FVector Direction = (HitLocation - OwnerLoc).GetSafeNormal2D();

    // Центр декали — ровно на половине дальности
    FVector DecalLoc = OwnerLoc + Direction * (CurrentTargetingMaxCastRange * 0.5f);
    DecalLoc.Z = HitLocation.Z;

    CurrentTargetLocation = OwnerLoc + Direction * CurrentTargetingMaxCastRange;

    const float Yaw = FMath::Atan2(Direction.Y, Direction.X) * (180.f / PI) + -90.f;

    // Квадрат со стороной = ConeRange → материал сам маскирует конус
    const float HalfSize = CurrentTargetingMaxCastRange * 0.5f;

    UpdateDecalTransform(DecalLoc, HalfSize, HalfSize, FRotator(-90.f, Yaw, 0.f));
}

bool UHR_AbilityTargetingComponent::GetGroundLocationUnderCursor(FVector& OutLocation) const
{
    AHR_PlayerController* PC = Cast<AHR_PlayerController>(GetPlayerController());
    if (!IsValid(PC)) return false;

    FVector MouseWorldLocation, MouseWorldDirection;
    if (!PC->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
        return false;

    // Трейс только по WorldStatic, персонажи игнорируются
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    // Игнорируем все Pawn'ы на сцене
    TArray<AActor*> AllPawns;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), AllPawns);
    Params.AddIgnoredActors(AllPawns);

    FHitResult HitResult;
    const FVector TraceStart = MouseWorldLocation;
    const FVector TraceEnd   = MouseWorldLocation + MouseWorldDirection * 10000.f;

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        ECC_WorldStatic, // Только статик, павны не участвуют
        Params
    );

    if (!bHit) return false;

    OutLocation = HitResult.ImpactPoint;
    OutLocation.Z += 5.f;
    return true;
}

void UHR_AbilityTargetingComponent::ShowDecal(float Radius, EHR_AbilityTargetingType Type, UMaterialInterface* Material)
{
    if (!IsValid(TargetingDecal))
    {
        TargetingDecal = NewObject<UDecalComponent>(GetOwner());
        TargetingDecal->RegisterComponent();
    }

    // Сбрасываем кэш и применяем новый материал
    CachedDynMat = nullptr;

    if (IsValid(Material))
    {
        // Создаём динамический инстанс сразу здесь
        CachedDynMat = UMaterialInstanceDynamic::Create(Material, TargetingDecal);
        TargetingDecal->SetDecalMaterial(CachedDynMat);
    }

    TargetingDecal->SetWorldRotation(FRotator(-90.f, 0.f, 0.f));
    TargetingDecal->DecalSize = FVector(128.f, Radius, Radius);
    TargetingDecal->SetVisibility(true);
}

void UHR_AbilityTargetingComponent::HideDecal()
{
    if (IsValid(TargetingDecal))
        TargetingDecal->SetVisibility(false);
}

void UHR_AbilityTargetingComponent::UpdateDecalTransform(
    const FVector& Location, float HalfLength, float HalfWidth, const FRotator& Rotation)
{
    if (!IsValid(TargetingDecal)) return;
    TargetingDecal->SetWorldLocation(Location + FVector(0, 0, 50.f));
    TargetingDecal->SetWorldRotation(Rotation);
    // X = глубина проекции, Y = вдоль направления, Z = поперёк
    TargetingDecal->DecalSize = FVector(128.f, HalfWidth, HalfLength);
}

APlayerController* UHR_AbilityTargetingComponent::GetPlayerController() const
{
    const APawn* Pawn = Cast<APawn>(GetOwner());
    return Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
}
