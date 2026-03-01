// 

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemTooltipWidget.generated.h"

class UItemData;
class UTextBlock;
/**
 * 
 */
UCLASS()
class HEROESOFTHEREARGUARD_API UItemTooltipWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescription;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemType;

	void ShowTooltip(UItemData* Data, FVector2D ScreenPos);
	void HideTooltip();
};
