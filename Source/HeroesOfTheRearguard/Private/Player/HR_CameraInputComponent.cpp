// 

#include "Player/HR_CameraInputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Framework/Application/SlateApplication.h"

UHR_CameraInputComponent::UHR_CameraInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Публичный API — вызывается из контроллера
// ─────────────────────────────────────────────────────────────────────────────

void UHR_CameraInputComponent::OnRMBPressed()
{
	bRMBHeld = true;
	HideCursor();
	EnableCharacterFollowCamera();
}

void UHR_CameraInputComponent::OnRMBReleased()
{
	bRMBHeld = false;
	DisableCharacterFollowCamera();

	// Курсор возвращаем только если ЛКМ тоже отпущена
	if (!bLMBHeld)
	{
		ShowCursor();
	}
}

void UHR_CameraInputComponent::OnLMBPressed()
{
	bLMBHeld = true;
	HideCursor();

}

void UHR_CameraInputComponent::OnLMBReleased()
{
	bLMBHeld = false;

	if (!bRMBHeld)
	{
		ShowCursor();
	}
}

bool UHR_CameraInputComponent::CanRotateCamera() const
{
	return bRMBHeld || bLMBHeld;
}

// ─────────────────────────────────────────────────────────────────────────────
// Курсор
// ─────────────────────────────────────────────────────────────────────────────

void UHR_CameraInputComponent::HideCursor()
{
	APlayerController* PC = GetPC();
	if (!PC) return;
	
	CurrentCursorPosition = GetCurrentMousePosition();

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