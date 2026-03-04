// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HR_UnitTargetingComponent.generated.h"

class IHR_Targetable;
class UAbilitySystemComponent;
class UDecalComponent;
class UMaterialInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetChanged, AActor*, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetOfTargetChanged, AActor*, NewTargetOfTarget);


USTRUCT(BlueprintType)
struct FHR_TargetInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly)
	float Health = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float MaxHealth = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Mana = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float MaxMana = 0.f;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> TargetOfTarget = nullptr;

	bool IsValid() const { return ::IsValid(TargetActor); }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEROESOFTHEREARGUARD_API UHR_UnitTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHR_UnitTargetingComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Crash|UnitTarget|Target")
	AActor* TryTargetUnderCursor();

	UFUNCTION(BlueprintCallable, Category = "Crash|UnitTarget|Target")
	void SetTarget(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Crash|UnitTarget|Target")
	void ClearTarget();

	UFUNCTION(BlueprintCallable, Category = "Crash|UnitTarget|Target")
	AActor* CycleTarget(float SearchRadius = 2000.f);

	UFUNCTION(BlueprintPure, Category = "Crash|UnitTarget|Target")
	bool HasTarget() const { return IsValid(CurrentTarget); }

	UFUNCTION(BlueprintPure, Category = "Crash|UnitTarget|UnitTarget|Target")
	AActor* GetCurrentTarget() const { return CurrentTarget; }

	UFUNCTION(BlueprintCallable, Category = "Crash|UnitTarget|Target")
	FHR_TargetInfo GetTargetInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Crash|UnitTarget|Target")
	FHR_TargetInfo GetTargetOfTargetInfo() const;

	UPROPERTY(BlueprintAssignable, Category = "Crash|UnitTarget|Target")
	FOnTargetChanged OnTargetChanged;

	UPROPERTY(BlueprintAssignable, Category = "Crash|UnitTarget|Target")
	FOnTargetOfTargetChanged OnTargetOfTargetChanged;

	UPROPERTY(EditDefaultsOnly, Category = "Crash|UnitTarget|Settings")
	float MaxTraceDistance = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Crash|UnitTarget|Settings")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

	UPROPERTY(EditDefaultsOnly, Category = "Crash|UnitTarget|Settings")
	bool bAutoClearInvalidTarget = true;

private:

	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> CachedTargetOfTarget = nullptr;

	int32 CycleIndex = 0;
	
	UPROPERTY() UDecalComponent* TargetDecal;
	UPROPERTY(EditDefaultsOnly, Category="Crash|UnitTarget|Visualization") 
	UMaterialInterface* TargetDecalMaterial;

	APlayerController* GetPlayerController() const;
	FHR_TargetInfo BuildTargetInfo(AActor* Actor) const;
	bool IsValidTarget(AActor* Actor) const;
	void UpdateTargetOfTarget();
	void UpdateVisualization(AActor* OldTarget);
};
