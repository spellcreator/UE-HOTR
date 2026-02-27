#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Core/HR_GameplayAbility.h"
#include "HR_Ability_Charge.generated.h"

class AHR_PlayerCharacter;

/**
 * UChargeAbility
 *
 * Рывок: запускает LaunchCharacter и завершается при приземлении.
 * Урона не наносит → наследуется от UHR_GameplayAbility (не от CombatAbility).
 * Если в будущем нужен урон при столкновении — сменить базовый класс на UHR_CombatAbility.
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UChargeAbility : public UHR_GameplayAbility
{
	GENERATED_BODY()

public:
	UChargeAbility();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	// Монтаж рывка (опционально — если нет, просто импульс)
	UPROPERTY(EditDefaultsOnly, Category="Crash|Charge|Animation")
	TObjectPtr<UAnimMontage> ChargeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Charge")
	float ChargeStrength = 2000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Crash|Charge")
	float ChargeZVelocity = 100.f;

private:
	FVector GetChargeDirection(ACharacter* Character) const;
	void LaunchCharge(ACharacter* Character);

	UFUNCTION() void OnLanded(const FHitResult& Hit);
	UFUNCTION() void OnMontageEnded();
};