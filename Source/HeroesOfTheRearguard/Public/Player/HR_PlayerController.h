// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HR_PlayerController.generated.h"

class UInputMappingContext;
class USpringArmComponent;
class UInputAction;
struct FInputActionValue;
struct FGameplayTag;

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
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input") 
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Movement") TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Movement") TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Movement") TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Movement") TObjectPtr<UInputAction> CameraBoomAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Abilities") TObjectPtr<UInputAction> LMBAbilityAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input|Abilities") TObjectPtr<UInputAction> ChargeAction;
	
	
	void Jump();
	void StopJumping();
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	void Zoom(const FInputActionValue& Value);
	
	void ActivateAbility(const FGameplayTag& AbilityTag) const;
	
	void LMBAbility();
	
	void ChargeAbility();
	
	bool isAlive() const;
	
};
