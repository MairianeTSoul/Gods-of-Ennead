// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskController.h"
#include "Components/WidgetComponent.h"

UTaskController::UTaskController()
{
}

UTaskController::~UTaskController()
{
	m_task = nullptr;
}

void UTaskController::init()
{
	m_task = NewObject<UTask>(this, m_taskTemplate);

	if (widgetClass)
	{
		m_widget = CreateWidget<UTaskUserWidget>(GetWorld(), widgetClass);
		m_widget->AddToViewport();
		m_widget->setTask(m_task);
	}
}

void UTaskController::tick(float dt)
{

}
