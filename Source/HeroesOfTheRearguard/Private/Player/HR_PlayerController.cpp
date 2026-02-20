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
#include "Player/HR_AbilityTargetingComponent.h"

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
	
	// Подтверждение таргета — отдельная кнопка ЛКМ
	EnhancedInputComponent->BindAction(ConfirmTargetingAction, ETriggerEvent::Started,
		this, &AHR_PlayerController::ConfirmTargeting);

	// Отмена
	EnhancedInputComponent->BindAction(CancelTargetingAction, ETriggerEvent::Started,
		this, &AHR_PlayerController::CancelCurrentTargeting);
	
	
	//Abilities
	EnhancedInputComponent->BindAction(LMBAbilityAction, ETriggerEvent::Triggered,
	this, &AHR_PlayerController::LMBAbility);
	EnhancedInputComponent->BindAction(ChargeAction, ETriggerEvent::Started,
		this, &AHR_PlayerController::ChargeAbility);
	EnhancedInputComponent->BindAction(JumpAttackAction, ETriggerEvent::Started,
		this, &AHR_PlayerController::JumpAttack);
	
}

void AHR_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	APawn* PlayerPawn = GetPawn();
	if (!PlayerPawn) return;
	
	CameraBoom = PlayerPawn->FindComponentByClass<USpringArmComponent>();
	
	// Targeting 
	
	TargetingComponent = NewObject<UHR_AbilityTargetingComponent>(PlayerPawn);
	TargetingComponent->RegisterComponent();

	// Подписываемся
	TargetingComponent->OnTargetingConfirmed.AddDynamic(
		this, &AHR_PlayerController::OnTargetingConfirmed);
	TargetingComponent->OnTargetingCancelled.AddDynamic(
		this, &AHR_PlayerController::OnTargetingCancelled);
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

void AHR_PlayerController::ConfirmTargeting()
{
	if (!TargetingComponent->IsTargeting()) return;

	UAbilitySystemComponent* ASC = 
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (!ASC) return;

	const FGameplayTag Tag = TargetingComponent->GetPendingAbilityTag();
    
	TargetingComponent->ConfirmTargeting(); // Скрывает декаль, бродкастит локацию
    
	ActivateAbilityByAssetTag(ASC, Tag);
}

void AHR_PlayerController::LMBAbility()
{
	//ActivateAbility(HRTags::HRAbilities::LMBAbility);
	
	TryActivateOrBeginTargeting(HRTags::HRAbilities::LMBAbility);
}

void AHR_PlayerController::ChargeAbility()
{
	
	//ActivateAbility(HRTags::HRAbilities::ChargeAbility);
	
	TryActivateOrBeginTargeting(HRTags::HRAbilities::ChargeAbility);
}

void AHR_PlayerController::JumpAttack()
{
	TryActivateOrBeginTargeting(HRTags::HRAbilities::JumpAttack);
}

bool AHR_PlayerController::isAlive() const
{
	AHR_BaseCharacter* BaseCharacter = Cast<AHR_BaseCharacter>(GetPawn());
	if (!IsValid(BaseCharacter)) return false;
	return BaseCharacter->IsAlive();
}

void AHR_PlayerController::TryActivateOrBeginTargeting(const FGameplayTag& AbilityTag)
{
	if (!isAlive()) return;

	UAbilitySystemComponent* ASC = 
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (!ASC) return;

	// Если идёт прицеливание — любая способность отменяет его и начинает своё
	if (TargetingComponent->IsTargeting())
	{
		TargetingComponent->CancelTargeting();
		// Если нажали ту же самую — просто отменяем (toggle)
		if (TargetingComponent->GetPendingAbilityTag() == AbilityTag) return;
	}

	UHR_GameplayAbility* AbilityCDO = FindAbilityByTag(ASC, AbilityTag);
	if (!AbilityCDO) return;

	if (AbilityCDO->RequiresTargeting())
	{
		TargetingComponent->BeginTargeting(AbilityTag, AbilityCDO);
	}
	else
	{
		ActivateAbilityByAssetTag(ASC, AbilityTag);
	}
}

void AHR_PlayerController::OnTargetingConfirmed(FVector TargetLocation)
{
	UAbilitySystemComponent* ASC = 
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (!ASC) return;

	// Передаём TargetData в ASC через GameplayEvent
	FGameplayEventData EventData;
	EventData.Instigator = GetPawn();
	EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromLocations(
		FGameplayAbilityTargetingLocationInfo(), // Source (можно заполнить)
		MakeTargetLocationInfo(TargetLocation)
	);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetPawn(),
		TargetingComponent->GetPendingAbilityTag(), // тег способности как Event тег
		EventData
	);
    
	// ИЛИ просто активируем, а способность сама заберёт локацию из компонента
	// (зависит от архитектуры конкретной способности)
	ASC->TryActivateAbilitiesByTag(
		TargetingComponent->GetPendingAbilityTag().GetSingleTagContainer()
	);
}

void AHR_PlayerController::OnTargetingCancelled()
{
	// Ничего специального — UI уже обновился через делегат
}

void AHR_PlayerController::CancelCurrentTargeting()
{
	if (TargetingComponent->IsTargeting())
		TargetingComponent->CancelTargeting();
}

UHR_GameplayAbility* AHR_PlayerController::FindAbilityByTag(
	UAbilitySystemComponent* ASC, const FGameplayTag& Tag) const
{
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (!IsValid(Spec.Ability)) continue;
		if (Spec.Ability->GetAssetTags().HasTagExact(Tag))
		{
			return Cast<UHR_GameplayAbility>(Spec.Ability);
		}
	}
	return nullptr;
}

// Утилита для TargetData
FGameplayAbilityTargetingLocationInfo AHR_PlayerController::MakeTargetLocationInfo(
	const FVector& Location) const
{
	FGameplayAbilityTargetingLocationInfo Info;
	Info.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	Info.LiteralTransform = FTransform(Location);
	return Info;
}

void AHR_PlayerController::ActivateAbilityByAssetTag(UAbilitySystemComponent* ASC, const FGameplayTag& Tag) const
{
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (!IsValid(Spec.Ability)) continue;
		if (Spec.Ability->GetAssetTags().HasTagExact(Tag))
		{
			ASC->TryActivateAbility(Spec.Handle);
			return;
		}
	}
}
