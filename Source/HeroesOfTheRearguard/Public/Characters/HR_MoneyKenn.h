// 

#pragma once

#include "CoreMinimal.h"
#include "HR_BaseCharacter.h"
#include "HR_MoneyKenn.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class HEROESOFTHEREARGUARD_API AHR_MoneyKenn : public AHR_BaseCharacter
{
	GENERATED_BODY()

public:
	AHR_MoneyKenn();
	
	virtual void GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const override;
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
};
