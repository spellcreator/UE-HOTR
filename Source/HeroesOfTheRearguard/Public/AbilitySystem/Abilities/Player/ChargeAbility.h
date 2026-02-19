// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HR_GameplayAbility.h"
#include "ChargeAbility.generated.h"

class AHR_PlayerController;
class AHR_PlayerCharacter;
/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UChargeAbility : public UHR_GameplayAbility
{
	GENERATED_BODY()
	
public:
	UChargeAbility();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	
	TWeakObjectPtr<AHR_PlayerCharacter> OwningPlayer;
	TWeakObjectPtr<AHR_PlayerController> OwningController;
	
protected:	
	// Сила рывка (импульс)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Settings")
	float ChargeStrength = 2000.0f;

	// Продолжительность рывка
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Settings")
	float ChargeDuration = 0.3f;

	// Высота рывка (Z-компонента)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Settings")
	float ChargeZVelocity = 100.0f;

	// Минимальная скорость персонажа для определения направления
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Settings")
	float MinimumVelocityThreshold = 10.0f;

	// Использовать направление камеры, если персонаж стоит на месте
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Settings")
	bool bUseCameraDirectionWhenIdle = true;

	// Кулдаун между рывками (в секундах)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge Settings")
	float ChargeCooldown = 2.0f;

private:
	// Получить направление для рывка
	FVector GetChargeDirection(ACharacter* PlayerCharacter) const;

	// Применить импульс к персонажу
	void ApplyChargeImpulse(ACharacter* PlayerCharacter, const FVector& Direction);
	
	void AddCollisionResponse(ACharacter* PlayerCharacter, const ECollisionResponse CollisionResponse) const;

	
	// Завершение способности
	UFUNCTION()
	void FinishCharge();

	FTimerHandle ChargeTimerHandle;
	
	UFUNCTION()
	void OnCharacterLanded(const FHitResult& Hit);

};
