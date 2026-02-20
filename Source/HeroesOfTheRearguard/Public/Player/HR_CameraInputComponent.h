// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HR_CameraInputComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEROESOFTHEREARGUARD_API UHR_CameraInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHR_CameraInputComponent();


	// ─── Вызывается из контроллера при инпуте ────────────────────────────────

	UFUNCTION(BlueprintCallable, Category="Crash|Camera")
	void OnRMBPressed();

	UFUNCTION(BlueprintCallable, Category="Crash|Camera")
	void OnRMBReleased();

	UFUNCTION(BlueprintCallable, Category="Crash|Camera")
	void OnLMBPressed();

	UFUNCTION(BlueprintCallable, Category="Crash|Camera")
	void OnLMBReleased();

	/**
	 * Контроллер вызывает это в Look() перед тем как применить AddYawInput/AddPitchInput.
	 * Возвращает true — вращаем камеру, false — игнорируем инпут.
	 */
	UFUNCTION(BlueprintPure, Category="Crash|Camera")
	bool CanRotateCamera() const;
	
private:

	bool bRMBHeld = false;
	bool bLMBHeld = false;

	/** Позиция мыши до скрытия — восстанавливаем при показе */
	//FVector2D SavedMousePosition = FVector2D::ZeroVector;
	FVector2D CurrentCursorPosition;

	// ─── Внутренняя логика ────────────────────────────────────────────────────

	void HideCursor();
	void ShowCursor();

	/** Персонаж смотрит туда же куда камера (ПКМ-режим) */
	void EnableCharacterFollowCamera();

	/** Персонаж живёт своей жизнью (дефолт / ЛКМ-режим) */
	void DisableCharacterFollowCamera();

	// ─── Хелперы ────────────────────────────────────────
	FVector GetCurrentMousePosition();
	APlayerController* GetPC() const;
	ACharacter*        GetCharacterOwner() const;
};
