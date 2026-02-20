// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HR_GameplayAbility.h"
#include "GameFramework/PlayerController.h"
#include "HR_PlayerController.generated.h"

class UHR_AbilityTargetActorComponent;
class UInputMappingContext;
class USpringArmComponent;
class UInputAction;
struct FInputActionValue;
struct FGameplayTag;
class UHR_AbilityTargetingComponent;

/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API AHR_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void SetupInputComponent() override;
	
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crash|Camera|Settings")
	float ZoomSpeed = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crash|Camera|Settings")
	float ArmMin = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crash|Camera|Settings")
	float ArmMax = 800.f;
	
private:
	
	// Targeting 
	
	UPROPERTY(VisibleAnywhere, Category="Crash|Targeting")
	TObjectPtr<UHR_AbilityTargetingComponent> TargetingComponent;
	
	void TryActivateOrBeginTargeting(const FGameplayTag& AbilityTag);
	void OnTargetingConfirmed(FVector TargetLocation);
	void OnTargetingCancelled();
	void CancelCurrentTargeting(); // Привязать на ПКМ / Escape
	UHR_GameplayAbility* FindAbilityByTag(UAbilitySystemComponent* ASC, const FGameplayTag& Tag) const;
	FGameplayAbilityTargetingLocationInfo MakeTargetLocationInfo(const FVector& Location) const;
	void ActivateAbilityByAssetTag(UAbilitySystemComponent* ASC, const FGameplayTag& Tag) const;

	// Inputs 
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input") 
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Movement") TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Movement") TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Movement") TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Movement") TObjectPtr<UInputAction> CameraBoomAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Abilities") TObjectPtr<UInputAction> ConfirmTargetingAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Abilities") TObjectPtr<UInputAction> CancelTargetingAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Abilities") TObjectPtr<UInputAction> LMBAbilityAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Abilities") TObjectPtr<UInputAction> ChargeAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Abilities") TObjectPtr<UInputAction> JumpAttackAction;
	
	bool isAlive() const;
	void Jump();
	void StopJumping();
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	void Zoom(const FInputActionValue& Value);
	
	void ConfirmTargeting();
	
	void LMBAbility();
	
	void ChargeAbility();
	
	void JumpAttack();
	
	
};
