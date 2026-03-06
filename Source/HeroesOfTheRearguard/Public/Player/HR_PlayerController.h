// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Core/HR_GameplayAbility.h"
#include "GameFramework/PlayerController.h"
#include "HR_PlayerController.generated.h"

class UHR_UnitTargetingComponent;
class UInventoryWidget;
class UInventoryComponent;
class UHR_CameraInputComponent;
class UHR_AbilityTargetActorComponent;
class UInputMappingContext;
class USpringArmComponent;
class UInputAction;
struct FInputActionValue;
struct FGameplayTag;
class UHR_AbilityTargetingComponent;

enum class EPlayerInputMode
{
	Default,
	Targeting,
	UI
};

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crash|PlayerController|Camera|Settings")
	float ZoomSpeed = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crash|PlayerController|Camera|Settings")
	float ArmMin = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crash|PlayerController|Camera|Settings")
	float ArmMax = 800.f;

	
private:
	EPlayerInputMode CurrentInputMode = EPlayerInputMode::Default;
	
	// ─── Компоненты ───────────────────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, Category="Crash|PlayerController|Camera")
	TObjectPtr<UHR_CameraInputComponent> CameraInputComponent;

	UPROPERTY(VisibleAnywhere, Category="Crash|PlayerController|Targeting")
	TObjectPtr<UHR_AbilityTargetingComponent> TargetingComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Crash|PlayerController|Target")
	TObjectPtr<UHR_UnitTargetingComponent> UnitTargetingComponent;
	
	
	UPROPERTY()
	TObjectPtr<UInventoryWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	// ─── Input Actions ────────────────────────────────────────────────────────

	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;
	
	// Basic Action
	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Movement")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Movement")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Camera")
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Camera")
	TObjectPtr<UInputAction> CameraBoomAction;

	// Mouse Action
	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Camera")
	TObjectPtr<UInputAction> RMB_Action;

	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Camera")
	TObjectPtr<UInputAction> LMB_Action;
	
	// Ability Targeting
	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Targeting")
	TObjectPtr<UInputAction> ConfirmTargetingAction;

	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Targeting")
	TObjectPtr<UInputAction> CancelTargetingAction;
	
	// UnitTargeting
	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Targeting")
	TObjectPtr<UInputAction> TabTargetAction;
	
	// Ability 
	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Abilities")
	TObjectPtr<UInputAction> ChargeAction;
	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Abilities")
	TObjectPtr<UInputAction> AttackAction;
	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Abilities")
	TObjectPtr<UInputAction> JumpAttackAction;
	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Abilities")
	TObjectPtr<UInputAction> BladeFuryAction;
	
	// INVENTORY
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerController|Input|Abilities")
	TObjectPtr<UInputAction> InventoryAction;
	
	

	// ─── Handlers ─────────────────────────────────────────────────────────────

	bool isAlive() const;

	//Movement
	void Jump();
	void StopJumping();
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	//AbilityTargeting
	void ConfirmTargeting();
	void CancelCurrentTargeting();
	UFUNCTION()
	void OnTargetingConfirmed(FVector TargetLocation);
	UFUNCTION()
	void OnTargetingCancelled();
	
	// UnitTargeting
	void GetUnitUnderCursor();
	void TabTarget();
	UFUNCTION()
	void OnUnitTargetChanged(AActor* NewTarget);
	
	//Camera
	void OnRMBPressed_Internal();
	void OnRMBReleased_Internal();
	void OnLMBPressed_Internal();
	void OnLMBReleased_Internal();
	void Zoom(const FInputActionValue& Value);

	//Ability
	void LMBAbility();
	void ChargeAbility();
	void JumpAttack();
	void BladeFury();

	void TryActivateOrBeginTargeting(const FGameplayTag& AbilityTag);
	
	// inve
	void ToggleInventory();
};