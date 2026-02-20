// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "AbilitySystem/Abilities/HR_AbilityTargetingType.h"
#include "HR_AbilityTargetingComponent.generated.h"


class UDecalComponent;
class UHR_GameplayAbility;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetingConfirmed, FVector, TargetLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetingCancelled);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEROESOFTHEREARGUARD_API UHR_AbilityTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UHR_AbilityTargetingComponent();
	    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    /** Начать фазу прицеливания для способности */
    UFUNCTION(BlueprintCallable, Category="Crash|Targeting")
    void BeginTargeting(const FGameplayTag& AbilityTag, UHR_GameplayAbility* Ability);

    /** Подтвердить прицеливание (2е нажатие) */
    UFUNCTION(BlueprintCallable, Category="Crash|Targeting")
    void ConfirmTargeting();

    /** Отменить прицеливание (ПКМ / Escape) */
    UFUNCTION(BlueprintCallable, Category="Crash|Targeting")
    void CancelTargeting();

    bool IsTargeting() const { return bIsTargeting; }
    FGameplayTag GetPendingAbilityTag() const { return PendingAbilityTag; }

    UPROPERTY(BlueprintAssignable)
    FOnTargetingConfirmed OnTargetingConfirmed;

    UPROPERTY(BlueprintAssignable)
    FOnTargetingCancelled OnTargetingCancelled;

    // ── Визуал ───────────────────────────────────────────────
    
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CachedDynMat;
    
    //* Материал декали для AOE-прицела #2#
    UPROPERTY(EditDefaultsOnly, Category="Crash|Targeting|Visuals")
    TObjectPtr<UMaterialInterface> GroundTargetDecalMaterial;

     //* Материал для направленной способности #2#  
    UPROPERTY(EditDefaultsOnly, Category="Crash|Targeting|Visuals")
    TObjectPtr<UMaterialInterface> DirectionalDecalMaterial;
    

    // Цвет декали в нормальном состоянии #1#
    UPROPERTY(EditDefaultsOnly, Category="Crash|Targeting|Visuals")
    FLinearColor ValidTargetColor = FLinearColor(0.f, 0.8f, 1.f, 0.6f);

    // Цвет декали если цель вне зоны или недоступна #1#
    UPROPERTY(EditDefaultsOnly, Category="Crash|Targeting|Visuals")
    FLinearColor InvalidTargetColor = FLinearColor(1.f, 0.2f, 0.f, 0.6f);
   

private:

    bool bIsTargeting = false;
    FGameplayTag PendingAbilityTag;
    EHR_AbilityTargetingType CurrentTargetingType;
    float CurrentRadius = 300.f;
    float CurrentRange = 1200.f;

    FVector CurrentTargetLocation = FVector::ZeroVector;
    bool bTargetIsValid = false;

    UPROPERTY()
    TObjectPtr<UDecalComponent> TargetingDecal;

    UPROPERTY()
    TObjectPtr<UDecalComponent> RangeDecal; // Опционально: показывать дальность

    APlayerController* GetPlayerController() const;
	

	void UpdateGroundTargetLocation();
    void UpdateDirectionalArc();
	
	void ShowDecal(float Radius, EHR_AbilityTargetingType Type, UMaterialInterface* Material);
    void HideDecal();
    void UpdateDecalTransform(const FVector& Location, float Radius);
    void UpdateDecalColor(bool bValid);
    
    
    bool GetGroundLocationUnderCursor(FVector& OutLocation) const;
	
};
