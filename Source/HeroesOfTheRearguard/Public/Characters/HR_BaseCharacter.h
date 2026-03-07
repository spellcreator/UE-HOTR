// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "TargetSystem/Interface/HR_Targetable.h"

#include "HR_BaseCharacter.generated.h"

struct FOnAttributeChangeData;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FASCInitialized, UAbilitySystemComponent*, ASC, UAttributeSet*, AS);

UCLASS(Abstract)
class HEROESOFTHEREARGUARD_API AHR_BaseCharacter : public ACharacter, public IAbilitySystemInterface, public IHR_Targetable
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
	
	/*UPROPERTY(EditAnywhere, Category = "Crash|AI")
	float SearchRange{1000.f};*/

	virtual bool CanBeTargeted_Implementation() const override;
	virtual FText GetTargetDisplayName_Implementation() const override;
	virtual UAbilitySystemComponent* GetTargetASC_Implementation() const override;
	virtual FVector GetTargetIndicatorLocation_Implementation() const override;
	// ---- Selection Decal (used by UnitTargetingComponent) ----

	UPROPERTY(VisibleAnywhere, Category = "Crash|BaseCharacter|SelectionDecal")
	TObjectPtr<UDecalComponent> SelectionDecalComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Crash|BaseCharacter|SelectionDecal")
	TObjectPtr<UMaterialInterface> SelectionDecalMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Crash|BaseCharacter|SelectionDecal")
	FVector SelectionDecalSize = FVector(200.f, 128.f, 128.f);
	
protected:	
	
	UPROPERTY(EditAnywhere, Category = "Crash|BaseCharacter|Target")
	FText TargetDisplayName;
	
	void GiveStartupAbilities();
	
	void InitializeAttributes() const;
	
	void OnHealthChange(const FOnAttributeChangeData& AttributeChangeData); 
	virtual void HandleDeath();
	
private:
	
	UPROPERTY(EditDefaultsOnly, Category= "Crash|BaseCharacter|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category="Crash|BaseCharacter|Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Replicated)
	bool bAlive = true;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Replicated)
	bool bCharged = false;

};
