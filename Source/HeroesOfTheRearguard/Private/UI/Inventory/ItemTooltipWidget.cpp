// 


#include "UI/Inventory/ItemTooltipWidget.h"

#include "Components/TextBlock.h"
#include "Invenotry/ItemData.h"

void UItemTooltipWidget::ShowTooltip(UItemData* Data, FVector2D ScreenPos)
{
	if (!Data) { HideTooltip(); return; }

	ItemName->SetText(Data->Name);
	ItemDescription->SetText(Data->Description);

	// Тип через UEnum
	const UEnum* EnumPtr = StaticEnum<EHR_ItemType>();
	ItemType->SetText(FText::FromString(
		EnumPtr->GetDisplayNameTextByValue((int64)Data->ItemType).ToString()));

	SetPositionInViewport(ScreenPos);
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UItemTooltipWidget::HideTooltip()
{
}
