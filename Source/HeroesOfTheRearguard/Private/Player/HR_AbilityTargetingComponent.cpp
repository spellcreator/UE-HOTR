
#include "Player/HR_AbilityTargetingComponent.h"
#include "AbilitySystem/Abilities/HR_GameplayAbility.h"
#include "Components/DecalComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

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
    CurrentTargetingType = Ability->TargetingType;
    CurrentRadius = Ability->TargetingRadius;
    CurrentRange = Ability->TargetingRange;

    // Применяем визуал из способности
    if (IsValid(Ability->TargetingDecalMaterial))
    {
        ShowDecal(CurrentRadius, CurrentTargetingType, Ability->TargetingDecalMaterial);
    }
    else
    {
        // Фолбэк на дефолтный материал компонента
        ShowDecal(CurrentRadius, CurrentTargetingType, Ability->TargetingDecalMaterial);
    }

    SetComponentTickEnabled(true);
}

void UHR_AbilityTargetingComponent::ConfirmTargeting()
{
    if (!bIsTargeting) return;

    if (bTargetIsValid)
    {
        const FVector ConfirmedLocation = CurrentTargetLocation;
        HideDecal();
        bIsTargeting = false;
        SetComponentTickEnabled(false);
        
        OnTargetingConfirmed.Broadcast(ConfirmedLocation);
    }
    // Если цель невалидна — не подтверждаем, даём игроку поправить
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
    if (GetGroundLocationUnderCursor(HitLocation))
    {
        // Ограничиваем по дальности от персонажа
        const AActor* Owner = GetOwner();
        if (IsValid(Owner))
        {
            const FVector OwnerLoc = Owner->GetActorLocation();
            const FVector ToTarget = HitLocation - OwnerLoc;
            if (ToTarget.Size2D() > CurrentRange)
            {
                HitLocation = OwnerLoc + ToTarget.GetSafeNormal2D() * CurrentRange;
                // Переопределяем Z через трассировку
                FHitResult RangeClampHit;
                FVector Start = HitLocation + FVector(0,0,500);
                FVector End = HitLocation - FVector(0,0,1000);
                if (GetWorld()->LineTraceSingleByChannel(RangeClampHit, Start, End, ECC_WorldStatic))
                    HitLocation = RangeClampHit.Location;
                
                bTargetIsValid = false;
            }
            else
            {
                bTargetIsValid = true;
            }
        }

        CurrentTargetLocation = HitLocation;
        UpdateDecalTransform(HitLocation, CurrentRadius);
        UpdateDecalColor(bTargetIsValid);
    }
}

void UHR_AbilityTargetingComponent::UpdateDirectionalArc()
{
    // Для направленных способностей — показываем конус от персонажа в сторону курсора
    FVector HitLocation;
    if (!GetGroundLocationUnderCursor(HitLocation)) return;

    const AActor* Owner = GetOwner();
    if (!IsValid(Owner)) return;

    const FVector OwnerLoc = Owner->GetActorLocation();
    FVector Direction = (HitLocation - OwnerLoc).GetSafeNormal2D();
    
    // Ставим декаль на дальность CurrentRange от персонажа
    const FVector DecalLoc = OwnerLoc + Direction * (CurrentRange * 0.5f);
    CurrentTargetLocation = OwnerLoc + Direction * CurrentRange;
    bTargetIsValid = true;

    UpdateDecalTransform(DecalLoc, CurrentRadius);
    UpdateDecalColor(true);
}

bool UHR_AbilityTargetingComponent::GetGroundLocationUnderCursor(FVector& OutLocation) const
{
    APlayerController* PC = GetPlayerController();
    if (!PC) return false;

    FHitResult HitResult;
    if (PC->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldStatic), 
                                              true, HitResult))
    {
        OutLocation = HitResult.Location;
        return true;
    }
    return false;
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

void UHR_AbilityTargetingComponent::UpdateDecalTransform(const FVector& Location, float Radius)
{
    if (!IsValid(TargetingDecal)) return;
    TargetingDecal->SetWorldLocation(Location + FVector(0, 0, 50.f));
    TargetingDecal->DecalSize = FVector(128.f, Radius, Radius);
}

void UHR_AbilityTargetingComponent::UpdateDecalColor(bool bValid)
{
    if (!IsValid(TargetingDecal)) return;

    UMaterialInstanceDynamic* DynMat = 
        Cast<UMaterialInstanceDynamic>(TargetingDecal->GetDecalMaterial());
    
    if (!IsValid(DynMat))
    {
        DynMat = TargetingDecal->CreateDynamicMaterialInstance();
    }

    if (IsValid(DynMat))
    {
        DynMat->SetVectorParameterValue(TEXT("Color"), 
            bValid ? ValidTargetColor : InvalidTargetColor);
    }
}

APlayerController* UHR_AbilityTargetingComponent::GetPlayerController() const
{
    const APawn* Pawn = Cast<APawn>(GetOwner());
    return Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
}




void UHR_AbilityTargetingComponent::GroundLocation()
{
    FHitResult HitResult;
    FVector StartLocation, EndLocation;
    
    APlayerController* PC = GetPlayerController();
    PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
    PC->DeprojectMousePositionToWorld(StartLocation, EndLocation);
}