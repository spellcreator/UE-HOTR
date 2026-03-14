// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HR_HitboxShape.generated.h"

UENUM(BlueprintType)
enum class EHR_HitboxShape : uint8
{
	Sphere   UMETA(DisplayName = "Sphere"),   // Сферический оверлап
	Cone     UMETA(DisplayName = "Cone"),     // Конус (сфера + фильтр по углу)
	Box      UMETA(DisplayName = "Box"),      // Бокс-оверлап
};
