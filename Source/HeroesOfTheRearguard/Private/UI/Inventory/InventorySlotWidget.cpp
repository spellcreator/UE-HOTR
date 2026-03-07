// 
#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Invenotry/ItemData.h"
#include "Invenotry/InventorySlot.h"
#include "Blueprint/DragDropOperation.h"
#include "Characters/HR_InventoryComponent.h"
#include "Components/Border.h"
#include "Components/PanelWidget.h"
#include "UI/Inventory/InventoryWidget.h"


void UInventorySlotWidget::UpdateSlot(const FInventorySlot& SlotData)
{
	if (SlotData.IsEmpty())
	{
		ItemIcon->SetVisibility(ESlateVisibility::Hidden);
		CountText->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	ItemIcon->SetVisibility(ESlateVisibility::Visible);
	ItemIcon->SetBrushFromTexture(SlotData.ItemData->Icon);

	// Показываем счётчик только если стак > 1
	if (SlotData.Count > 1)
	{
		CountText->SetVisibility(ESlateVisibility::Visible);
		CountText->SetText(FText::AsNumber(SlotData.Count));
	}
	else
	{
		CountText->SetVisibility(ESlateVisibility::Hidden);
	}
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& Geo, const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseButtonDown(Geo, MouseEvent);
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot %d clicked!"), SlotIndex);

		return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return FReply::Unhandled();
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& Geo, const FPointerEvent& MouseEvent,
	UDragDropOperation*& Op)
{
	auto* DragOp = NewObject<UDragDropOperation>();
	DragOp->Payload = this;  // передаём себя как payload
	DragOp->DefaultDragVisual = this; // визуал при перетаскивании
	DragOp->Pivot = EDragPivot::CenterCenter;

	Op = DragOp;

	UE_LOG(LogTemp, Warning, TEXT("Dragging slot %d"), SlotIndex);
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& Geo, const FDragDropEvent& DragDropEvent,
	UDragDropOperation* Op)
{
	auto* FromSlot = Cast<UInventorySlotWidget>(Op->Payload);
	if (!FromSlot) return false;

	UE_LOG(LogTemp, Warning, TEXT("Drop: slot %d -> slot %d"), 
		FromSlot->SlotIndex, SlotIndex);

	// Находим компонент инвентаря и свапаем
	auto* InvWidget = Cast<UInventoryWidget>(FromSlot->GetParent()->GetParent()->GetParent());
	// Проще — храни ссылку на компонент
    
	return true;
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& Geo, const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseEnter(Geo, MouseEvent);

	// 1. Подсветка рамки
	SlotBorder->SetBrushColor(FLinearColor(1.f, 1.f, 1.f, 0.3f)); // светлее

	// 2. Показать тултип
	if (InventoryComp && !InventoryComp->Slots[SlotIndex].IsEmpty())
	{
		FVector2D ScreenPos = Geo.GetAbsolutePosition();
		OnSlotHovered.Broadcast(InventoryComp->Slots[SlotIndex].ItemData, ScreenPos);
	}
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseLeave(MouseEvent);

	// 1. Убрать подсветку
	SlotBorder->SetBrushColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f)); // дефолт

	// 2. Скрыть тултип
	OnSlotUnhovered.Broadcast();
}
