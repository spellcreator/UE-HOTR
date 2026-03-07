// 


#include "Tasks/HR_AbilityTask_JumpToLocation.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UHR_AbilityTask_JumpToLocation* UHR_AbilityTask_JumpToLocation::JumpToLocation(UGameplayAbility* OwningAbility,
	FVector TargetLocation, float Duration, float ArcHeight, UCurveFloat* HeightCurve, UCurveFloat* LateralCurve, float NearingLandTime)
{
		
	UHR_AbilityTask_JumpToLocation* JumpTask = NewAbilityTask<UHR_AbilityTask_JumpToLocation>(OwningAbility);
	JumpTask->EndLocation   = TargetLocation;
	JumpTask->TotalDuration = FMath::Max(Duration, 0.1f);
	JumpTask->MaxArcHeight  = FMath::Max(ArcHeight, 0.f); // ✅ Units, не [0..1]
	JumpTask->HeightCurve   = HeightCurve;   // nullptr = дефолтная парабола
	JumpTask->LateralCurve  = LateralCurve;  // nullptr = линейное движение
	JumpTask->bTickingTask  = true;
	JumpTask->NearingLandOffset = FMath::Max(NearingLandTime, 0.f);
	JumpTask->bNearingLandFired = false;
	return JumpTask;
}

void UHR_AbilityTask_JumpToLocation::Activate()
{
	Super::Activate();
	
	OwnerCharacter = Cast<ACharacter>(GetAvatarActor());
	if (!OwnerCharacter)
	{
		OnInterrupted.Broadcast();
		EndTask();
		return;
	}

	StartLocation = OwnerCharacter->GetActorLocation();
	ElapsedTime   = 0.f;

	// Отключаем физику на время прыжка
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
}

void UHR_AbilityTask_JumpToLocation::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	
	if (!OwnerCharacter)
	{
		OnInterrupted.Broadcast();
		EndTask();
		return;
	}

	ElapsedTime += DeltaTime;
	const float Alpha = FMath::Clamp(ElapsedTime / TotalDuration, 0.f, 1.f);

	const FVector NewPosition = ComputePosition(Alpha);
	OwnerCharacter->SetActorLocation(NewPosition, true); // sweep=true — учитываем коллизии

	// Поворачиваем персонажа в сторону движения
	const FVector Direction = (EndLocation - StartLocation).GetSafeNormal2D();
	if (!Direction.IsNearlyZero())
	{
		OwnerCharacter->SetActorRotation(Direction.Rotation());
	}
	
	const float NearingLandAlpha = FMath::Clamp((TotalDuration - NearingLandOffset) / TotalDuration, 0.f, 1.f);
	if (!bNearingLandFired && Alpha >= NearingLandAlpha)
	{
		bNearingLandFired = true;
		OnNearingLand.Broadcast();
	}

	if (Alpha >= 1.f)
	{
		// Поворачиваем к камере перед завершением
		if (AController* Controller = OwnerCharacter->GetController())
		{
			const FRotator ControlRotation = Controller->GetControlRotation();
			const FRotator NewRotation = FRotator(0.f, ControlRotation.Yaw, 0.f); 
			OwnerCharacter->SetActorRotation(NewRotation);
		}
		// Возвращаем нормальное движение
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		OnCompleted.Broadcast();
		EndTask();
	}
}

FVector UHR_AbilityTask_JumpToLocation::ComputePosition(float Alpha) const
{
	// --- Горизонталь ---
	float LateralAlpha = Alpha;
	if (LateralCurve)
	{
		// Кривая может сделать S-образное движение, рывок, замедление и т.д.
		LateralAlpha = FMath::Clamp(LateralCurve->GetFloatValue(Alpha), 0.f, 1.f);
	}
	const FVector LinearPos = FMath::Lerp(StartLocation, EndLocation, LateralAlpha);

	// --- Вертикаль ---
	float HeightAlpha = 0.f;
	if (HeightCurve)
	{
		// Берём значение кривой — она и задаёт форму прыжка
		HeightAlpha = HeightCurve->GetFloatValue(Alpha);
	}
	else
	{
		// Дефолт: парабола sin(π*t)
		HeightAlpha = FMath::Sin(PI * Alpha);
	}

	return LinearPos + FVector(0.f, 0.f, MaxArcHeight * HeightAlpha);
}

void UHR_AbilityTask_JumpToLocation::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	// Гарантируем восстановление режима движения при любом завершении
	if (OwnerCharacter && OwnerCharacter->GetCharacterMovement()->MovementMode == MOVE_Flying)
	{
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	Super::OnDestroy(bInOwnerFinished);
}