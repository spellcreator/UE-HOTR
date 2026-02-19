// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HR_BaseCharacter.h"
#include "HR_PlayerCharacter.generated.h"

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
	
private:
	
	UPROPERTY(VisibleAnywhere, Category="Crash|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	UPROPERTY(VisibleAnywhere, Category="Crash|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;
	
};
