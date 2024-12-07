// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskController.h"
#include "Components/WidgetComponent.h"

UTaskController::UTaskController()
{
}

UTaskController::~UTaskController()
{
}

void UTaskController::init()
{
	if (widgetClass)
	{
		//TODO Investigate how to create widget from non UE classes
		//m_widget = CreateWidget(widgetClass);
	}
}

void UTaskController::tick(float dt)
{

}
