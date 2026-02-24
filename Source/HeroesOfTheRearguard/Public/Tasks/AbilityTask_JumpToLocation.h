// 

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_JumpToLocation.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpToLocationDelegate);

/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UAbilityTask_JumpToLocation : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable) FJumpToLocationDelegate OnCompleted;
	UPROPERTY(BlueprintAssignable) FJumpToLocationDelegate OnInterrupted;

	/**
	 * @param HeightCurve - кривая по оси Z (X=время[0..1], Y=высота[0..1])
	 *                      nullptr = прямолинейное движение без дуги
	 * @param LateralCurve - кривая горизонтального смещения (опционально, для S-кривых)
	 *                      nullptr = линейное горизонтальное движение  
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
			  meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_JumpToLocation* JumpToLocation(
		UGameplayAbility* OwningAbility,
		FVector           TargetLocation,
		float             Duration     = 0.8f,
		float             ArcHeight    = 200.f,  // максимальная высота в Units
		UCurveFloat*      HeightCurve  = nullptr,
		UCurveFloat*      LateralCurve = nullptr);

	virtual void Activate()          override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	UPROPERTY() ACharacter*   OwnerCharacter;
	UPROPERTY() UCurveFloat*  HeightCurve;   // форма дуги по высоте
	UPROPERTY() UCurveFloat*  LateralCurve;  // боковое смещение

	FVector StartLocation;
	FVector EndLocation;
	float   TotalDuration;
	float   ElapsedTime;
	float   MaxArcHeight;

	FVector ComputePosition(float Alpha) const;
};
