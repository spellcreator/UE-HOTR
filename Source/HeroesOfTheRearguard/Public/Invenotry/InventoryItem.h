// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryItem.generated.h"

class UItemData;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UItemData> ItemData;

	UPROPERTY()
	int32 Count = 0;
};