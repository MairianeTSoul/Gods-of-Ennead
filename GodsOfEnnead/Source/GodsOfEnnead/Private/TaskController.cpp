// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskController.h"

#include "Task.h"
#include "Components/WidgetComponent.h"

UTaskController::UTaskController()
{
	GenerateTask();
	widgetClass = LoadClass<UTaskUserWidget>(nullptr, TEXT("/Game/BP/UI/WBP_Task.WBP_Task_C"));
}

UTaskController::~UTaskController()
{
	Task = nullptr;
}

void UTaskController::init(UWorld* World)
{
	if (widgetClass)
	{
		TaskWidget = CreateWidget<UTaskUserWidget>(World, widgetClass);
		TaskWidget->AddToViewport();
		TaskWidget->setTask(Task);
	}
}

void UTaskController::GenerateTask()
{
	const ETaskPattern RandomPattern = static_cast<ETaskPattern>(FMath::RandRange(0, static_cast<int32>(ETaskPattern::NAME)));
	const int32 RandomCount = FMath::RandRange(3, 5);
	ETaskType RandomType;
	if (RandomPattern == ETaskPattern::NAME)
	{
		RandomType = ETaskType::SET;
	}
	else
	{
		RandomType = static_cast<ETaskType>(FMath::RandRange(0, static_cast<int32>(ETaskType::SET)));
	}

	Task = NewObject<UTask>();
	Task->InitializeTask(RandomPattern, RandomCount, RandomType);
}
