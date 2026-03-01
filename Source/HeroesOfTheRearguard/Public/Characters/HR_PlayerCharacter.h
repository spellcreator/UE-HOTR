// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HR_BaseCharacter.h"
#include "HR_PlayerCharacter.generated.h"

class UInventoryComponent;
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
	
	// Inventory
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY()
	TObjectPtr<UInventoryWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;


	

	
private:
	
	UPROPERTY(VisibleAnywhere, Category="Crash|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	UPROPERTY(VisibleAnywhere, Category="Crash|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;
	
};
