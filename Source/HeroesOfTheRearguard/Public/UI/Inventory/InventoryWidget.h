// 

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UItemTooltipWidget;
class UHR_InventoryComponent;
class UInventorySlotWidget;
class UUniformGridPanel;
/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* SlotGrid;

	UPROPERTY(meta = (BindWidget))
	UItemTooltipWidget* Tooltip;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

	UPROPERTY()
	TArray<UInventorySlotWidget*> SlotWidgets;

	void InitInventory(UHR_InventoryComponent* InvComp);
	UFUNCTION()
	void RefreshAllSlots();
	
	UFUNCTION()
	void ShowTooltip(UItemData* ItemData, FVector2D ScreenPos);

	UFUNCTION()
	void HideTooltip();

private:
	UPROPERTY()
	UHR_InventoryComponent* InventoryComp;

	int32 Columns = 5;
};
