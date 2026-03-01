// 


#include "UI/Inventory/InventoryWidget.h"

#include "Characters/InventoryComponent.h"
#include "Components/UniformGridPanel.h"
#include "UI/Inventory/InventorySlotWidget.h"
#include "UI/Inventory/ItemTooltipWidget.h"


void UInventoryWidget::InitInventory(UInventoryComponent* InvComp)
{
	InventoryComp = InvComp;
	SlotGrid->ClearChildren();
	SlotWidgets.Empty();
	
	// Гарантируем что слоты готовы
	if (InvComp->Slots.Num() == 0)
	{
		InvComp->Slots.SetNum(InvComp->MaxSlots);
	}
       

	if (!SlotWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SlotWidgetClass is NULL! Set it in WBP_InventoryWidget defaults."));
		return;
	}

	for (int32 i = 0; i < InvComp->MaxSlots; ++i)
	{
		auto* SlotW = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
		SlotW->SlotIndex = i;
		SlotW->InventoryComp = InvComp;

		// Привязка тултипа
		SlotW->OnSlotHovered.AddDynamic(this, &UInventoryWidget::ShowTooltip);
		SlotW->OnSlotUnhovered.AddDynamic(this, &UInventoryWidget::HideTooltip);

		SlotGrid->AddChildToUniformGrid(SlotW, i / Columns, i % Columns);
		SlotWidgets.Add(SlotW);
	}

	InvComp->OnInventoryUpdated.AddDynamic(this, &UInventoryWidget::RefreshAllSlots);
	RefreshAllSlots();
}

void UInventoryWidget::RefreshAllSlots()
{
	UE_LOG(LogTemp, Warning, TEXT("RefreshAllSlots called. Slots: %d, Widgets: %d"), 
	InventoryComp->Slots.Num(), SlotWidgets.Num());

	for (int32 i = 0; i < SlotWidgets.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot[%d]: %s x%d"), i,
			InventoryComp->Slots[i].ItemData ? *InventoryComp->Slots[i].ItemData->Name.ToString() : TEXT("Empty"),
			InventoryComp->Slots[i].Count);
		SlotWidgets[i]->UpdateSlot(InventoryComp->Slots[i]);
	}
}

void UInventoryWidget::ShowTooltip(UItemData* ItemData, FVector2D ScreenPos)
{
	if (Tooltip && ItemData)
	{
		Tooltip->ShowTooltip(ItemData, ScreenPos);
	}
}

void UInventoryWidget::HideTooltip()
{
	if (Tooltip)
	{
		Tooltip->HideTooltip();
	}
}