// 

#include "Player/HR_CameraInputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

UHR_CameraInputComponent::UHR_CameraInputComponent()
{
	// Tick выключен по умолчанию, включаем только во время SoftLook-возврата
	PrimaryComponentTick.bCanEverTick  = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Tick — работает только во время плавного возврата после отпускания ЛКМ
// ─────────────────────────────────────────────────────────────────────────────

void UHR_CameraInputComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bSoftLookReturning) return;

	ACharacter* Char = GetCharacterOwner();
	APlayerController* PC = GetPC();
	if (!Char || !PC) return;

	FRotator ControlRot = PC->GetControlRotation();
	const float TargetYaw = Char->GetActorRotation().Yaw;

	// FInterpTo не знает про цикличность углов и может поехать длинным путём.
	// FindDeltaAngleDegrees возвращает кратчайшую дугу в диапазоне [-180, 180],
	// поэтому всегда выбираем короткий маршрут.
	const float Delta  = FMath::FindDeltaAngleDegrees(ControlRot.Yaw, TargetYaw);
	const float NewYaw = ControlRot.Yaw + FMath::FInterpTo(0.f, Delta, DeltaTime, CameraReturnSpeed);

	ControlRot.Yaw = NewYaw;
	PC->SetControlRotation(ControlRot);

	// Возврат завершён
	if (FMath::Abs(FMath::FindDeltaAngleDegrees(NewYaw, TargetYaw)) < CameraReturnThreshold)
	{
		FinishSoftLookReturn();
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// Инициализация
// ─────────────────────────────────────────────────────────────────────────────

void UHR_CameraInputComponent::InitializeDefaultState()
{
	// Дефолт = HardLook: персонаж всегда смотрит по направлению камеры.
	// Стрейф-анимации отвечают за движение влево/вправо/назад.
	ApplyHardLook();
}

// ─────────────────────────────────────────────────────────────────────────────
// Публичный API
// ─────────────────────────────────────────────────────────────────────────────

void UHR_CameraInputComponent::OnRMBPressed()
{
	bRMBHeld = true;
	
	// Прерываем SoftLook-возврат если он шёл
	if (bSoftLookReturning)
	{
		bSoftLookReturning = false;
		SetComponentTickEnabled(false);
	}
	if (!bLMBHeld)
	{
		CurrentCursorPosition = GetCurrentMousePosition();
	}

	HideCursor();
	//ApplyHardLook(); // RMB = мгновенный поворот к камере (уже активен в дефолте, явно для ясности)
	FinishSoftLookReturn();
}

void UHR_CameraInputComponent::OnRMBReleased()
{
	bRMBHeld = false;

	if (bLMBHeld)
	{
		// ЛКМ ещё зажата — ПКМ успела переключить в HardLook, восстанавливаем заморозку
		ApplySoftLookFreeze();
	}
	else
	{
		ShowCursor();
		// Возвращаемся в дефолт — HardLook уже активен
	}
}

void UHR_CameraInputComponent::OnLMBPressed()
{
	bLMBHeld = true;
	bLMBHeldFreeze = true;
	

	// Прерываем SoftLook-возврат если он шёл
	if (bSoftLookReturning)
	{
		bSoftLookReturning = false;
		SetComponentTickEnabled(false);
	}

	if (!bRMBHeld)
	{
		CurrentCursorPosition = GetCurrentMousePosition();
	}

	HideCursor();
	ApplySoftLookFreeze(); // Замораживаем форвард персонажа
}

void UHR_CameraInputComponent::OnLMBReleased()
{
	bLMBHeld = false;
	

	if (bRMBHeld)
	{
		// ПКМ ещё зажата — просто возвращаем HardLook, SoftLook возврат не нужен
		ApplyHardLook();
		return;
	}

	ShowCursor();

	// Запускаем плавный возврат камеры за спину персонажа
	if (bIsNeedReturn)
	{
		BeginSoftLookReturn();
	}
}

bool UHR_CameraInputComponent::IsSoftLookActive() const
{
	// Форвард персонажа заморожен пока ЛКМ зажата ИЛИ идёт плавный возврат
	return bLMBHeldFreeze || bSoftLookReturning;
}

bool UHR_CameraInputComponent::CanRotateCamera() const
{
	return bRMBHeld || bLMBHeld;
}

// ─────────────────────────────────────────────────────────────────────────────
// Режимы ориентации персонажа
// ─────────────────────────────────────────────────────────────────────────────

void UHR_CameraInputComponent::ApplyHardLook()
{
	ACharacter* Char = GetCharacterOwner();
	if (!Char) return;
	// Персонаж мгновенно следует за yaw контроллера (= камеры)
	Char->bUseControllerRotationYaw = true;

	if (UCharacterMovementComponent* CMC = Char->GetCharacterMovement())
	{
		CMC->bOrientRotationToMovement  = false; // НЕ поворачивать к движению
		CMC->bUseControllerDesiredRotation = false;
	}
	
}

void UHR_CameraInputComponent::ApplySoftLookFreeze()
{
	ACharacter* Char = GetCharacterOwner();
	if (!Char) return;

	// Отключаем все автоматические повороты — актор остаётся там где был
	Char->bUseControllerRotationYaw = false;

	if (UCharacterMovementComponent* CMC = Char->GetCharacterMovement())
	{
		CMC->bOrientRotationToMovement     = false;
		CMC->bUseControllerDesiredRotation = false;
	}
}

void UHR_CameraInputComponent::BeginSoftLookReturn()
{
	// Персонаж остаётся замороженным — камера едет к нему сама через Tick.
	// bUseControllerDesiredRotation не нужен — мы двигаем ControlRotation напрямую.
	bSoftLookReturning = true;
	SetComponentTickEnabled(true);
}

void UHR_CameraInputComponent::FinishSoftLookReturn()
{
	bSoftLookReturning = false;
	SetComponentTickEnabled(false);

	// Возвращаем HardLook — snap в точное направление камеры
	ApplyHardLook();
}


// ─────────────────────────────────────────────────────────────────────────────
// Курсор
// ─────────────────────────────────────────────────────────────────────────────

void UHR_CameraInputComponent::HideCursor()
{
	APlayerController* PC = GetPC();
	if (!PC) return;
	
	PC->bShowMouseCursor       = false;
	PC->bEnableClickEvents     = false;
	PC->bEnableMouseOverEvents = false;

	FInputModeGameOnly GameOnlyMode;
	PC->SetInputMode(GameOnlyMode);
}

void UHR_CameraInputComponent::ShowCursor()
{
	APlayerController* PC = GetPC();
	if (!PC) return;

	PC->SetMouseLocation(CurrentCursorPosition.X,CurrentCursorPosition.Y);
	// Возвращаем InputMode обратно в GameAndUI — курсор снова свободен
	FInputModeGameAndUI GameAndUIMode;
	GameAndUIMode.SetHideCursorDuringCapture(false);
	PC->SetInputMode(GameAndUIMode);

	PC->bShowMouseCursor       = true;
	PC->bEnableClickEvents     = true;
	PC->bEnableMouseOverEvents = true;
	
}

// ─────────────────────────────────────────────────────────────────────────────
// Ориентация персонажа
// ─────────────────────────────────────────────────────────────────────────────

void UHR_CameraInputComponent::EnableCharacterFollowCamera()
{
	ACharacter* Char = GetCharacterOwner();
	if (!Char) return;

	Char->bUseControllerRotationYaw = true;

	if (UCharacterMovementComponent* CMC = Char->GetCharacterMovement())
	{
		CMC->bOrientRotationToMovement = false;
	}
}

void UHR_CameraInputComponent::DisableCharacterFollowCamera()
{
	ACharacter* Char = GetCharacterOwner();
	if (!Char) return;

	Char->bUseControllerRotationYaw = false;

	if (UCharacterMovementComponent* CMC = Char->GetCharacterMovement())
	{
		CMC->bOrientRotationToMovement = true;
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// Хелперы
// ─────────────────────────────────────────────────────────────────────────────

APlayerController* UHR_CameraInputComponent::GetPC() const
{
	// Компонент живёт на Pawn — идём через его контроллер
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return nullptr;
	return Cast<APlayerController>(OwnerPawn->GetController());
}

ACharacter* UHR_CameraInputComponent::GetCharacterOwner() const
{
	return Cast<ACharacter>(GetOwner());
}

FVector UHR_CameraInputComponent::GetCurrentMousePosition()
{
	APlayerController* PC = GetPC();
	if (!PC) return FVector();
	float MouseX;
	float MouseY;
	PC->GetMousePosition(MouseX, MouseY);
	const FVector MousePos = FVector(MouseX, MouseY,0);
	return MousePos;
}