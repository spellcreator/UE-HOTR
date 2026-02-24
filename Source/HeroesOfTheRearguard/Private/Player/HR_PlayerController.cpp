// 
#include "Player/HR_PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystem/HR_AbilitySystemComponent.h"
#include "Characters/HR_BaseCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayTags/HRTags.h"
#include "Player/HR_AbilityTargetingComponent.h"
#include "Player/HR_CameraInputComponent.h"

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
	EIC->BindAction(LookAction,       ETriggerEvent::Triggered, this, &AHR_PlayerController::Look);
	EIC->BindAction(CameraBoomAction, ETriggerEvent::Triggered, this, &AHR_PlayerController::Zoom);

	// Camera (ПКМ) — делегируем в компонент
	EIC->BindAction(RMBAction, ETriggerEvent::Started,   this, &AHR_PlayerController::OnRMBPressed_Internal);
	EIC->BindAction(RMBAction, ETriggerEvent::Completed, this, &AHR_PlayerController::OnRMBReleased_Internal);

	// Abilities (ЛКМ) — Started/Completed для курсора, Triggered для способности
	EIC->BindAction(LMBAbilityAction, ETriggerEvent::Started,   this, &AHR_PlayerController::OnLMBPressed_Internal);
	EIC->BindAction(LMBAbilityAction, ETriggerEvent::Completed, this, &AHR_PlayerController::OnLMBReleased_Internal);
	//EIC->BindAction(LMBAbilityAction, ETriggerEvent::Triggered, this, &AHR_PlayerController::LMBAbility);

	// Targeting
	EIC->BindAction(ConfirmTargetingAction, ETriggerEvent::Started, this, &AHR_PlayerController::ConfirmTargeting);
	EIC->BindAction(CancelTargetingAction,  ETriggerEvent::Started, this, &AHR_PlayerController::CancelCurrentTargeting);

	// Other Abilities
	EIC->BindAction(ChargeAction,     ETriggerEvent::Started, this, &AHR_PlayerController::ChargeAbility);
	EIC->BindAction(JumpAttackAction, ETriggerEvent::Started, this, &AHR_PlayerController::JumpAttack);
}

void AHR_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = GetPawn();
	if (!PlayerPawn) return;

	CameraBoom = PlayerPawn->FindComponentByClass<USpringArmComponent>();

	// Создаём CameraInputComponent и вешаем на Pawn,
	// чтобы он мог добраться до персонажа через GetOwner()
	CameraInputComponent = NewObject<UHR_CameraInputComponent>(PlayerPawn);
	CameraInputComponent->RegisterComponent();

	// Targeting
	TargetingComponent = NewObject<UHR_AbilityTargetingComponent>(PlayerPawn);
	TargetingComponent->RegisterComponent();
	
	TargetingComponent->OnTargetingConfirmed.AddDynamic(this, &AHR_PlayerController::OnTargetingConfirmed);
	TargetingComponent->OnTargetingCancelled.AddDynamic(this, &AHR_PlayerController::OnTargetingCancelled);

	// WoW default: курсор виден
	bShowMouseCursor       = true;
	bEnableClickEvents     = true;
	bEnableMouseOverEvents = true;
}

// ─────────────────────────────────────────────────────────────────────────────
// Camera input — тонкие обёртки, вся логика в компоненте
// ─────────────────────────────────────────────────────────────────────────────

void AHR_PlayerController::OnRMBPressed_Internal()
{
	if (CameraInputComponent) CameraInputComponent->OnRMBPressed();
}

void AHR_PlayerController::OnRMBReleased_Internal()
{
	if (CameraInputComponent) CameraInputComponent->OnRMBReleased();
}

void AHR_PlayerController::OnLMBPressed_Internal()
{
	if (CameraInputComponent) CameraInputComponent->OnLMBPressed();
}

void AHR_PlayerController::OnLMBReleased_Internal()
{
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
	if (!TargetingComponent->IsTargeting()) return;
	
	TargetingComponent->ConfirmTargeting();
}

void AHR_PlayerController::CancelCurrentTargeting()
{
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

// ─────────────────────────────────────────────────────────────────────────────
// Abilities
// ─────────────────────────────────────────────────────────────────────────────

void AHR_PlayerController::LMBAbility()
{
	if (!CameraInputComponent || !CameraInputComponent->CanRotateCamera()) return;

	TryActivateOrBeginTargeting(HRTags::HRAbilities::LMBAbility);
}

void AHR_PlayerController::ChargeAbility()
{
	TryActivateOrBeginTargeting(HRTags::HRAbilities::ChargeAbility);
}

void AHR_PlayerController::JumpAttack()
{
	TryActivateOrBeginTargeting(HRTags::HRAbilities::JumpAttack);
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

	if (AbilityCDO->RequiresTargeting()){}
	
	if (ASC->RequiresTargeting(AbilityTag))
	{
		TargetingComponent->BeginTargeting(AbilityTag, AbilityCDO);
	}
	else
	{
		ASC->TryActivateAbilityByTag(AbilityTag);
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

