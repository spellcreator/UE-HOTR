// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Invenotry/InventorySlot.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);


USTRUCT(BlueprintType)
struct FDebugFillEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UItemData* Item = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 Count = 1;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEROESOFTHEREARGUARD_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UInventoryComponent();

	// --- Настройки ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 MaxSlots = 20;

	// --- Данные ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventorySlot> Slots;

	// --- Делегат для UI ---
	UPROPERTY(BlueprintAssignable)
	FOnInventoryUpdated OnInventoryUpdated;

	// --- API ---
	UFUNCTION(BlueprintCallable)
	bool TryAddItem(UItemData* ItemData, int32 Amount = 1);

	UFUNCTION(BlueprintCallable)
	bool RemoveItem(int32 SlotIndex, int32 Amount = 1);

	UFUNCTION(BlueprintCallable)
	void SwapSlots(int32 IndexA, int32 IndexB);

	UFUNCTION(BlueprintPure)
	int32 GetItemCount(UItemData* ItemData) const;
	
	
	// DEBUG 
	UFUNCTION(BlueprintCallable, Category = "Inventory|Debug")
	void DebugFillInventory();

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Debug")
	TArray<FDebugFillEntry> DebugItems;

protected:
	virtual void BeginPlay() override;

private:
	// Ищет слот, где уже лежит этот предмет и есть место в стаке
	int32 FindStackableSlot(UItemData* ItemData) const;
	// Ищет первый пустой слот
	int32 FindEmptySlot() const;
};
