// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HR_BaseCharacter.h"
#include "HR_PlayerCharacter.generated.h"

class UHR_UnitTargetingComponent;
class UHR_AbilityTargetingComponent;
class UHR_CameraInputComponent;
class UHR_InventoryComponent;
class UInventoryWidget;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class HEROESOFTHEREARGUARD_API AHR_PlayerCharacter : public AHR_BaseCharacter
{
	GENERATED_BODY()

public:
	AHR_PlayerCharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	// ---- Components (were in Controller, now on Character) ----
	
	UHR_CameraInputComponent* GetCameraInputComponent();
	UHR_AbilityTargetingComponent* GetAbilityTargetingComponent();
	UHR_UnitTargetingComponent* GetUnitTargetingComponent();
	UHR_InventoryComponent* GetInventoryComponent();

	UPROPERTY()
	TObjectPtr<UInventoryWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Crash|PlayerCharacter|UI")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

private:
	
	UPROPERTY(VisibleAnywhere, Category = "Crash|PlayerCharacter|Camera")
	TObjectPtr<UHR_CameraInputComponent> CameraInputComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Crash|PlayerCharacter|Targeting")
	TObjectPtr<UHR_AbilityTargetingComponent> AbilityTargetingComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Crash|PlayerCharacter|UnitTarget")
	TObjectPtr<UHR_UnitTargetingComponent> UnitTargetingComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Crash|PlayerCharacter|Inventory")
	TObjectPtr<UHR_InventoryComponent> InventoryComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Crash|PlayerCharacter|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	UPROPERTY(VisibleAnywhere, Category="Crash|PlayerCharacter|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;
	
};
