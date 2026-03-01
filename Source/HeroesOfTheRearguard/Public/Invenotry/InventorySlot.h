// 

#pragma once
#include "ItemData.h"

#include "InventorySlot.generated.h"



USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UItemData* ItemData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	bool IsEmpty() const { return ItemData == nullptr || Count <= 0; }

	void Clear()
	{
		ItemData = nullptr;
		Count = 0;
	}
};
