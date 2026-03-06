// 
#include "Player/HR_PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystem/HR_AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Characters/HR_BaseCharacter.h"
#include "Characters/HR_PlayerCharacter.h"
#include "Characters/InventoryComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayTags/HRTags.h"
#include "Player/HR_AbilityTargetingComponent.h"
#include "Player/HR_CameraInputComponent.h"
#include "Player/HR_UnitTargetingComponent.h"
#include "UI/Inventory/InventoryWidget.h"

// ─────────────────────────────────────────────────────────────────────────────
// Setup
// ─────────────────────────────────────────────────────────────────────────────

void AHR_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem)) return;

	for (UInputMappingContext* Context : InputMappingContexts)
	{
		InputSubsystem->AddMappingContext(Context, 0);
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EIC)) return;

	// Movement
	EIC->BindAction(JumpAction,       ETriggerEvent::Started,   this, &AHR_PlayerController::Jump);
	EIC->BindAction(JumpAction,       ETriggerEvent::Completed, this, &AHR_PlayerController::StopJumping);
	EIC->BindAction(MoveAction,       ETriggerEvent::Triggered, this, &AHR_PlayerController::Move);

	// Camera (ПКМ) — делегируем в компонент
	EIC->BindAction(RMB_Action, ETriggerEvent::Started,   this, &AHR_PlayerController::OnRMBPressed_Internal);
	EIC->BindAction(RMB_Action, ETriggerEvent::Completed, this, &AHR_PlayerController::OnRMBReleased_Internal);
	EIC->BindAction(LMB_Action, ETriggerEvent::Started,   this, &AHR_PlayerController::OnLMBPressed_Internal);
	EIC->BindAction(LMB_Action, ETriggerEvent::Completed, this, &AHR_PlayerController::OnLMBReleased_Internal);
	EIC->BindAction(LookAction,       ETriggerEvent::Triggered, this, &AHR_PlayerController::Look);
	EIC->BindAction(CameraBoomAction, ETriggerEvent::Triggered, this, &AHR_PlayerController::Zoom);

	// Targeting
	//EIC->BindAction(LMB_Action, ETriggerEvent::Started, this, &AHR_PlayerController::ConfirmTargeting);
	//EIC->BindAction(LMB_Action, ETriggerEvent::Started, this, &AHR_PlayerController::OnLMBPressed_Internal);
	EIC->BindAction(CancelTargetingAction,  ETriggerEvent::Started, this, &AHR_PlayerController::CancelCurrentTargeting);
	
	EIC->BindAction(TabTargetAction, ETriggerEvent::Started,this, &AHR_PlayerController::TabTarget);

	// Abilities
	EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &AHR_PlayerController::LMBAbility);
	EIC->BindAction(ChargeAction,     ETriggerEvent::Started, this, &AHR_PlayerController::ChargeAbility);
	EIC->BindAction(JumpAttackAction, ETriggerEvent::Started, this, &AHR_PlayerController::JumpAttack);
	EIC->BindAction(BladeFuryAction, ETriggerEvent::Started, this, &AHR_PlayerController::BladeFury);
	
	// Inv
	EIC->BindAction(InventoryAction, ETriggerEvent::Started, this, &AHR_PlayerController::ToggleInventory);
	
}

void AHR_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	AHR_PlayerCharacter* PlayerCharacter = Cast<AHR_PlayerCharacter>(GetPawn());
	if (!PlayerCharacter) return;

	CameraBoom = PlayerCharacter->FindComponentByClass<USpringArmComponent>();

	// --- Fetch components from Character (they already exist as subobjects) ---
	CameraInputComponent   = PlayerCharacter->GetCameraInputComponent();
	TargetingComponent     = PlayerCharacter->GetAbilityTargetingComponent();
	UnitTargetingComponent = PlayerCharacter->GetUnitTargetingComponent();

	// Init camera component if needed
	if (CameraInputComponent)
	{
		CameraInputComponent->InitializeDefaultState();
	}

	// Bind targeting delegates
	if (TargetingComponent)
	{
		TargetingComponent->OnTargetingConfirmed.AddDynamic(this, &AHR_PlayerController::OnTargetingConfirmed);
		TargetingComponent->OnTargetingCancelled.AddDynamic(this, &AHR_PlayerController::OnTargetingCancelled);
	}

	// Bind unit targeting delegates
	if (UnitTargetingComponent)
	{
		UnitTargetingComponent->OnTargetChanged.AddDynamic(this, &AHR_PlayerController::OnUnitTargetChanged);
	}

	// Cursor
	bShowMouseCursor       = true;
	bEnableClickEvents     = true;
	bEnableMouseOverEvents = true;

	// Inventory UI
	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UInventoryWidget>(this, InventoryWidgetClass);
		InventoryWidget->InitInventory(PlayerCharacter->GetInventoryComponent());
		InventoryWidget->AddToViewport();
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}


void AHR_PlayerController::OnUnitTargetChanged(AActor* NewTarget)
{
	// Placeholder for highlight, sound, etc.
	// UI subscribes to UnitTargetingComponent->OnTargetChanged directly.
}

// ─────────────────────────────────────────────────────────────────────────────
// Camera input — тонкие обёртки, вся логика в компоненте
// ─────────────────────────────────────────────────────────────────────────────

void AHR_PlayerController::OnRMBPressed_Internal()
{
	//if (!CameraInputComponent || !CameraInputComponent->CanRotateCamera()) return;
	if (CameraInputComponent) CameraInputComponent->OnRMBPressed();
}

void AHR_PlayerController::OnRMBReleased_Internal()
{
	if (CameraInputComponent) CameraInputComponent->OnRMBReleased();
}

void AHR_PlayerController::OnLMBPressed_Internal()
{
	switch(CurrentInputMode)
	{
	case EPlayerInputMode::Default:
		if (CameraInputComponent) CameraInputComponent->OnLMBPressed();
		break;

	case EPlayerInputMode::Targeting:
		ConfirmTargeting();
		break;
	case EPlayerInputMode::UI:
		break;
	}
}

void AHR_PlayerController::OnLMBReleased_Internal()
{
	GetUnitUnderCursor();
	if (CameraInputComponent) CameraInputComponent->OnLMBReleased();
}

// ─────────────────────────────────────────────────────────────────────────────
// Movement
// ─────────────────────────────────────────────────────────────────────────────

void AHR_PlayerController::Jump()
{
	if (!IsValid(GetCharacter())) return;
	if (!isAlive()) return;
	GetCharacter()->Jump();
}

void AHR_PlayerController::StopJumping()
{
	if (!IsValid(GetCharacter())) return;
	if (!isAlive()) return;
	GetCharacter()->StopJumping();
}

void AHR_PlayerController::Move(const FInputActionValue& Value)
{
	if (!IsValid(GetPawn())) return;
	if (!isAlive()) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	// SoftLook (ЛКМ зажата или идёт возврат): берём замороженный форвард персонажа,
	// чтобы он продолжал бежать в ту же сторону независимо от поворота камеры.
	// HardLook / Default: берём форвард камеры как обычно.
	const bool bUseFrozenForward = CameraInputComponent && CameraInputComponent->IsSoftLookActive();
	const float YawSource = bUseFrozenForward
		? GetPawn()->GetActorRotation().Yaw
		: GetControlRotation().Yaw;

	const FRotator YawRotation(0, YawSource, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection,   MovementVector.X);
}

void AHR_PlayerController::Look(const FInputActionValue& Value)
{
	// Гейтинг через компонент — камера вращается только при зажатой ПКМ или ЛКМ
	if (!CameraInputComponent || !CameraInputComponent->CanRotateCamera()) return;
	if (!isAlive()) return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void AHR_PlayerController::Zoom(const FInputActionValue& Value)
{
	if (!isAlive()) return;
	if (!CameraBoom) return;

	const float Axis = Value.Get<float>();
	const float NewLen = FMath::Clamp(
		CameraBoom->TargetArmLength - Axis * ZoomSpeed,
		ArmMin,
		ArmMax
	);
	CameraBoom->TargetArmLength = NewLen;
}

// ─────────────────────────────────────────────────────────────────────────────
// Targeting
// ─────────────────────────────────────────────────────────────────────────────

void AHR_PlayerController::ConfirmTargeting()
{
	CurrentInputMode = EPlayerInputMode::Default;
	if (!TargetingComponent->IsTargeting()) return;
	
	TargetingComponent->ConfirmTargeting();
}

void AHR_PlayerController::CancelCurrentTargeting()
{
	CurrentInputMode = EPlayerInputMode::Default;
	if (TargetingComponent->IsTargeting())
		TargetingComponent->CancelTargeting();
}

void AHR_PlayerController::OnTargetingConfirmed(FVector TargetLocation)
{
	UAbilitySystemComponent* ASC = 
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (!ASC) return;

	const FGameplayTag Tag = TargetingComponent->GetPendingAbilityTag();

	/*// Проверяем что тег валиден
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, 
		FString::Printf(TEXT("Confirming with Tag: %s"), *Tag.ToString()));*/

	if (!Tag.IsValid()) return;

	FGameplayAbilityTargetData_LocationInfo* LocationData = 
		new FGameplayAbilityTargetData_LocationInfo();
	LocationData->TargetLocation.LocationType = 
		EGameplayAbilityTargetingLocationType::LiteralTransform;
	LocationData->TargetLocation.LiteralTransform = FTransform(TargetLocation);

	FGameplayEventData EventData;
	EventData.Instigator = GetPawn();
	EventData.TargetData.Add(LocationData);

	int32 TriggeredCount = ASC->HandleGameplayEvent(Tag, &EventData);
}

void AHR_PlayerController::OnTargetingCancelled()
{
	// Визуал уже скрыт компонентом — ничего дополнительного
}

void AHR_PlayerController::GetUnitUnderCursor()
{
	if (!UnitTargetingComponent) return;
	if (CurrentInputMode == EPlayerInputMode::Targeting) return;
	
	UnitTargetingComponent->TryTargetUnderCursor();
}

void AHR_PlayerController::TabTarget()
{
	if (!UnitTargetingComponent) return;
	UnitTargetingComponent->CycleTarget();
}

// ─────────────────────────────────────────────────────────────────────────────
// Abilities
// ─────────────────────────────────────────────────────────────────────────────

void AHR_PlayerController::LMBAbility()
{
	TryActivateOrBeginTargeting(HRTags::HRAbilities::LMBAbility);
}

void AHR_PlayerController::ChargeAbility()
{
	TryActivateOrBeginTargeting(HRTags::HRAbilities::ChargeAbility);
	
	AHR_PlayerCharacter* PlayerCharacter = Cast<AHR_PlayerCharacter>(GetPawn());
	if (!PlayerCharacter) return;
	
	// Убрать после тестов

	PlayerCharacter->GetInventoryComponent()->DebugFillInventory();
}

void AHR_PlayerController::JumpAttack()
{
	TryActivateOrBeginTargeting(HRTags::HRAbilities::JumpAttack);
}

void AHR_PlayerController::BladeFury()
{
	TryActivateOrBeginTargeting(HRTags::HRAbilities::BladeFury);
}

void AHR_PlayerController::TryActivateOrBeginTargeting(const FGameplayTag& AbilityTag)
{
	if (!isAlive()) return;
	
	AHR_BaseCharacter* Char = Cast<AHR_BaseCharacter>(GetPawn());
	if (!Char) return;

	UHR_AbilitySystemComponent* ASC = Cast<UHR_AbilitySystemComponent>(Char->GetAbilitySystemComponent());
	if (!ASC) return;

	if (TargetingComponent->IsTargeting())
	{
		TargetingComponent->CancelTargeting();
		if (TargetingComponent->GetPendingAbilityTag() == AbilityTag) return;
	}

	UHR_GameplayAbility* AbilityCDO = ASC->FindAbilityByTag(AbilityTag);
	if (!AbilityCDO) return;

	const EHR_AbilityTargetingType TargetType = AbilityCDO->TargetingData.TargetingType;

	if (TargetType == EHR_AbilityTargetingType::UnitTarget)
	{
		// UnitTarget: requires a selected unit target to activate
		if (!UnitTargetingComponent || !UnitTargetingComponent->HasTarget())
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
				TEXT("Need a target to use this ability"));
			return;
		}

		AActor* TargetActor = UnitTargetingComponent->GetCurrentTarget();

		FGameplayAbilityTargetData_ActorArray* ActorData = new FGameplayAbilityTargetData_ActorArray();
		ActorData->TargetActorArray.Add(TWeakObjectPtr<AActor>(TargetActor));

		FGameplayEventData EventData;
		EventData.Instigator = GetPawn();
		EventData.Target = TargetActor;
		EventData.TargetData.Add(ActorData);

		ASC->HandleGameplayEvent(AbilityTag, &EventData);
	}
	else if (AbilityCDO->RequiresTargeting())
	{
		CurrentInputMode = EPlayerInputMode::Targeting;
		TargetingComponent->BeginTargeting(AbilityTag, AbilityCDO);
	}
	else
	{
		ASC->TryActivateAbilityByTag(AbilityTag);
	}
}

void AHR_PlayerController::ToggleInventory()
{
	if (!InventoryWidget) return;

	bool bIsVisible = InventoryWidget->GetVisibility() == ESlateVisibility::Visible;

	if (bIsVisible)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("InventoryWidget Collapsed"));
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		SetInputMode(FInputModeGameOnly());
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("InventoryWidget Visible"));
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
		FInputModeGameAndUI Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(Mode);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// Utils
// ─────────────────────────────────────────────────────────────────────────────

bool AHR_PlayerController::isAlive() const
{
	AHR_BaseCharacter* BaseCharacter = Cast<AHR_BaseCharacter>(GetPawn());
	if (!IsValid(BaseCharacter)) return false;
	return BaseCharacter->IsAlive();
}

