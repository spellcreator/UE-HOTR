// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"

#include "HR_BaseCharacter.generated.h"

struct FOnAttributeChangeData;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FASCInitialized, UAbilitySystemComponent*, ASC, UAttributeSet*, AS);

UCLASS(Abstract)
class HEROESOFTHEREARGUARD_API AHR_BaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AHR_BaseCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual UAttributeSet* GetAttributeSet() const {return nullptr; };
	
	bool IsAlive() const {return bAlive;}
	void SetAlive(bool bAliveStatus) {bAlive = bAliveStatus;}
	
	bool InCharge() const {return bCharged;}
	void SetCharge(bool bChargeStatus) {bCharged = bChargeStatus;}
	
	UPROPERTY(BlueprintAssignable)
	FASCInitialized OnASCInitialized;
	
	UFUNCTION(BlueprintCallable, Category = "Crash|Death")
	virtual void HandleRespawn();
	UFUNCTION(BlueprintCallable, Category = "Crash|Attributes")
	void ResetAttributes();
	
	UFUNCTION(BlueprintImplementableEvent)
	void RotateToTarget(AActor* RotateTarget);
	
	UPROPERTY(EditAnywhere, Category = "Crash|AI")
	float SearchRange{1000.f};
	
protected:	
	
	void GiveStartupAbilities();
	
	void InitializeAttributes() const;
	
	void OnHealthChange(const FOnAttributeChangeData& AttributeChangeData); 
	virtual void HandleDeath();
	
private:
	
	UPROPERTY(EditDefaultsOnly, Category= "HR|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category="HR|Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="HR|Effects")
	TSubclassOf<UGameplayEffect> ResetAttributesEffect;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Replicated)
	bool bAlive = true;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Replicated)
	bool bCharged = false;

};
