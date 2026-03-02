// 


#include "Tasks/AbilityTask_PeriodicAction.h"

UAbilityTask_PeriodicAction* UAbilityTask_PeriodicAction::PeriodicAction(UGameplayAbility* OwningAbility, float Period,
	float TotalDuration, bool bFireImmediately)
{
	auto* Task = NewAbilityTask<UAbilityTask_PeriodicAction>(OwningAbility);
	Task->Period          = FMath::Max(Period, 0.05f);
	Task->TotalDuration   = TotalDuration;
	Task->bFireImmediately = bFireImmediately;
	Task->bTickingTask     = true;
	return Task;
}

void UAbilityTask_PeriodicAction::Activate()
{
	Super::Activate();
	
	if (bFireImmediately)
	{
		OnPeriodicAction.Broadcast();
		TimeSinceLastAction = 0.f;
	}
}

void UAbilityTask_PeriodicAction::TickTask(float DeltaTime)
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

void UAbilityTask_PeriodicAction::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}
