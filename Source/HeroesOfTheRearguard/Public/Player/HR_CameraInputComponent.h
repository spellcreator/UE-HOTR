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
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// ─── Инициализация ────────────────────────────────────────────────────────

	/** Вызвать из BeginPlay контроллера после получения Pawn */
	void InitializeDefaultState();

	// ─── Вызывается из контроллера при инпуте ────────────────────────────────

	UFUNCTION(BlueprintCallable, Category="Crash|Camera")
	void OnRMBPressed();

	UFUNCTION(BlueprintCallable, Category="Crash|Camera")
	void OnRMBReleased();

	UFUNCTION(BlueprintCallable, Category="Crash|Camera")
	void OnLMBPressed();

	UFUNCTION(BlueprintCallable, Category="Crash|Camera")
	void OnLMBReleased();
	bool IsSoftLookActive() const;

	/**
	 * Контроллер вызывает это в Look() перед тем как применить AddYawInput/AddPitchInput.
	 * Возвращает true — вращаем камеру, false — игнорируем инпут.
	 */
	UFUNCTION(BlueprintPure, Category="Crash|Camera")
	bool CanRotateCamera() const;
	
	// ─── Настройки SoftLook ───────────────────────────────────────────────────

	/** Скорость возврата камеры за спину персонажа после отпускания ЛКМ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crash|Camera|SoftLook")
	float CameraReturnSpeed = 5.f;

	/** Порог в градусах — при таком отклонении считаем возврат завершённым */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crash|Camera|SoftLook")
	float CameraReturnThreshold = 0.5f;
	
	/** Нужно ли возвращать камеру за спину игроку */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crash|Camera|SoftLook")
	bool bIsNeedReturn = false;
	
private:

	bool bRMBHeld = false;
	bool bLMBHeld = false;
	bool bLMBHeldFreeze = false;
	bool bSoftLookReturning = false; // идёт плавный возврат после отпускания ЛКМ

	/** Позиция мыши до скрытия — восстанавливаем при показе */
	//FVector2D SavedMousePosition = FVector2D::ZeroVector;
	FVector CurrentCursorPosition;

	// ─── Режимы ориентации ────────────────────────────────────────────────────

	/** Default / HardLook / RMB: персонаж мгновенно следует за yaw камеры */
	void ApplyHardLook();

	/** SoftLook held: форвард персонажа заморожен */
	void ApplySoftLookFreeze();

	/** SoftLook return: включаем плавную интерполяцию к камере */
	void BeginSoftLookReturn();

	/** Вызывается из Tick когда поворот завершён — снэп обратно в HardLook */
	void FinishSoftLookReturn();

	// ─── Курсор / InputMode ───────────────────────────────────────────────────

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
