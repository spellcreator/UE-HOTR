// 

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

class UInventoryComponent;
class UBorder;
class UTextBlock;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotHovered, UItemData*, ItemData, FVector2D, ScreenPos);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlotUnhovered);
/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CountText;

	UPROPERTY(meta = (BindWidget))
	UBorder* SlotBorder; // подсветка при ховере

	UPROPERTY(BlueprintReadOnly)
	int32 SlotIndex = -1;
	
	UPROPERTY()
	UInventoryComponent* InventoryComp;

	UFUNCTION(BlueprintCallable)
	void UpdateSlot(const FInventorySlot& SlotData);

	// Для Drag & Drop
	virtual FReply NativeOnMouseButtonDown(const FGeometry& Geo,
		const FPointerEvent& MouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& Geo,
		const FPointerEvent& MouseEvent, UDragDropOperation*& Op) override;
	virtual bool NativeOnDrop(const FGeometry& Geo,
		const FDragDropEvent& DragDropEvent, UDragDropOperation* Op) override;

	// Тултип
	virtual void NativeOnMouseEnter(const FGeometry& Geo,
		const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;
	
	UPROPERTY()
	FOnSlotHovered OnSlotHovered;

	UPROPERTY()
	FOnSlotUnhovered OnSlotUnhovered;
};
