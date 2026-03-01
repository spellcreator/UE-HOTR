// 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

enum class EHR_ItemType : uint8;

UCLASS()
class UItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	FName ItemID;

	UPROPERTY(EditDefaultsOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxStack = 1;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> WorldActorClass;
	
	UPROPERTY(EditDefaultsOnly)
	EHR_ItemType ItemType;
};
