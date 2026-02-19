// 
#include "Player/HR_PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "Characters/HR_BaseCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayTags/HRTags.h"

void AHR_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem)) return;
	
	for (UInputMappingContext* Context : InputMappingContexts)
	{
		InputSubsystem->AddMappingContext(Context,0);
	}
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent)) return;
	//Movement
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AHR_PlayerController::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AHR_PlayerController::StopJumping);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered ,this, &AHR_PlayerController::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered ,this, &AHR_PlayerController::Look);
	EnhancedInputComponent->BindAction(CameraBoomAction, ETriggerEvent::Triggered ,this, &AHR_PlayerController::Zoom);
	
	//Abilities
	EnhancedInputComponent->BindAction(LMBAbilityAction, ETriggerEvent::Triggered ,this, &AHR_PlayerController::LMBAbility);
	
	EnhancedInputComponent->BindAction(ChargeAction, ETriggerEvent::Started ,this, &AHR_PlayerController::ChargeAbility);
}

void AHR_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	APawn* PlayerPawn = GetPawn();
	if (!PlayerPawn) return;
	
	CameraBoom = PlayerPawn->FindComponentByClass<USpringArmComponent>();
}

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
	
	const FRotator YawRotation(0,GetControlRotation().Yaw,0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.X);
}

void AHR_PlayerController::Look(const FInputActionValue& Value)
{
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

void AHR_PlayerController::ActivateAbility(const FGameplayTag& AbilityTag) const
{
	if (!isAlive()) return;
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (!ASC) return;
	
	ASC->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
}

void AHR_PlayerController::LMBAbility()
{
	ActivateAbility(HRTags::HRAbilities::LMBAbility);
}

void AHR_PlayerController::ChargeAbility()
{
	
	ActivateAbility(HRTags::HRAbilities::ChargeAbility);
}

bool AHR_PlayerController::isAlive() const
{
	AHR_BaseCharacter* BaseCharacter = Cast<AHR_BaseCharacter>(GetPawn());
	if (!IsValid(BaseCharacter)) return false;
	return BaseCharacter->IsAlive();
}
