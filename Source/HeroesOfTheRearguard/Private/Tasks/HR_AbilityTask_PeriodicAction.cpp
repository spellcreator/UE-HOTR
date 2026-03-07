// 


#include "Tasks/HR_AbilityTask_PeriodicAction.h"

UHR_AbilityTask_PeriodicAction* UHR_AbilityTask_PeriodicAction::PeriodicAction(UGameplayAbility* OwningAbility, float Period,
                                                                               float TotalDuration, bool bFireImmediately)
{
	auto* Task = NewAbilityTask<UHR_AbilityTask_PeriodicAction>(OwningAbility);
	Task->Period          = FMath::Max(Period, 0.05f);
	Task->TotalDuration   = TotalDuration;
	Task->bFireImmediately = bFireImmediately;
	Task->bTickingTask     = true;
	return Task;
}

void UHR_AbilityTask_PeriodicAction::Activate()
{
	Super::Activate();
	
	if (bFireImmediately)
	{
		OnPeriodicAction.Broadcast();
		TimeSinceLastAction = 0.f;
	}
}

void UHR_AbilityTask_PeriodicAction::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	
	ElapsedTime += DeltaTime;
	TimeSinceLastAction += DeltaTime;

	// Проверяем завершение
	if (TotalDuration > 0.f && ElapsedTime >= TotalDuration)
	{
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	// Периодический вызов
	if (TimeSinceLastAction >= Period)
	{
		TimeSinceLastAction -= Period;
		OnPeriodicAction.Broadcast();
	}
}

void UHR_AbilityTask_PeriodicAction::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}
