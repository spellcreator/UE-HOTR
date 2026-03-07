// 


#include "Characters/HR_InventoryComponent.h"

#include "Invenotry/ItemData.h"

UHR_InventoryComponent::UHR_InventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    Slots.SetNum(MaxSlots);
}

void UHR_InventoryComponent::BeginPlay()
{
    Super::BeginPlay();
}

bool UHR_InventoryComponent::TryAddItem(UItemData* ItemData, int32 Amount)
{
    if (!ItemData || Amount <= 0) return false;

    int32 Remaining = Amount;

    // Фаза 1: заполняем существующие стаки
    while (Remaining > 0)
    {
        int32 StackIdx = FindStackableSlot(ItemData);
        if (StackIdx == INDEX_NONE) break;

        int32 SpaceInStack = ItemData->MaxStack - Slots[StackIdx].Count;
        int32 ToAdd = FMath::Min(Remaining, SpaceInStack);

        Slots[StackIdx].Count += ToAdd;
        Remaining -= ToAdd;
    }

    // Фаза 2: создаём новые стаки в пустых слотах
    while (Remaining > 0)
    {
        int32 EmptyIdx = FindEmptySlot();
        if (EmptyIdx == INDEX_NONE)
        {
            // Инвентарь полон — сообщаем, сколько не влезло
            UE_LOG(LogTemp, Warning, TEXT("Inventory full! %d items lost."), Remaining);
            break;
        }

        int32 ToAdd = FMath::Min(Remaining, ItemData->MaxStack);
        Slots[EmptyIdx].ItemData = ItemData;
        Slots[EmptyIdx].Count = ToAdd;
        Remaining -= ToAdd;
    }

    OnInventoryUpdated.Broadcast();
    
        UE_LOG(LogTemp, Warning, TEXT("TryAddItem done. Broadcasting. Listeners: %d"), 
        OnInventoryUpdated.IsBound() ? 1 : 0);
    OnInventoryUpdated.Broadcast();
    return Remaining == 0;
    
    
}

bool UHR_InventoryComponent::RemoveItem(int32 SlotIndex, int32 Amount)
{
    if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty())
        return false;

    Slots[SlotIndex].Count -= Amount;
    if (Slots[SlotIndex].Count <= 0)
    {
        Slots[SlotIndex].Clear();
    }

    OnInventoryUpdated.Broadcast();
    return true;
}

void UHR_InventoryComponent::SwapSlots(int32 IndexA, int32 IndexB)
{
    if (!Slots.IsValidIndex(IndexA) || !Slots.IsValidIndex(IndexB)) return;

    // Если одинаковый предмет — пробуем объединить стаки
    if (Slots[IndexA].ItemData == Slots[IndexB].ItemData && Slots[IndexB].ItemData != nullptr)
    {
        int32 Space = Slots[IndexB].ItemData->MaxStack - Slots[IndexB].Count;
        int32 Transfer = FMath::Min(Slots[IndexA].Count, Space);
        Slots[IndexB].Count += Transfer;
        Slots[IndexA].Count -= Transfer;
        if (Slots[IndexA].Count <= 0) Slots[IndexA].Clear();
    }
    else
    {
        Slots.Swap(IndexA, IndexB);
    }

    OnInventoryUpdated.Broadcast();
}

int32 UHR_InventoryComponent::GetItemCount(UItemData* ItemData) const
{
    int32 Total = 0;
    for (const auto& Slot : Slots)
    {
        if (Slot.ItemData == ItemData) Total += Slot.Count;
    }
    return Total;
}

int32 UHR_InventoryComponent::FindStackableSlot(UItemData* ItemData) const
{
    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        if (Slots[i].ItemData == ItemData && Slots[i].Count < ItemData->MaxStack)
            return i;
    }
    return INDEX_NONE;
}

int32 UHR_InventoryComponent::FindEmptySlot() const
{
    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        if (Slots[i].IsEmpty()) return i;
    }
    return INDEX_NONE;
}


void UHR_InventoryComponent::DebugFillInventory()
{
    for (auto& Slot : Slots)
    {
        Slot.Clear();
    }

    UE_LOG(LogTemp, Warning, TEXT("=== DebugFill: %d entries ==="), DebugItems.Num());

    for (const auto& Entry : DebugItems)
    {
        if (Entry.Item)
        {
            UE_LOG(LogTemp, Warning, TEXT("Adding: %s x%d"), 
                *Entry.Item->Name.ToString(), Entry.Count);
            TryAddItem(Entry.Item, Entry.Count);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Entry has NULL item!"));
        }
    }
}
