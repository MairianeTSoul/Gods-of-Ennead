// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskUserWidget.h"

bool URule::checkRule(FDataCardStruct& dataStruct)
{
	return false;
}

FString URule::getNameRule() const
{
	return nameRule;
}


//////////////////////////////////////////
bool UChooseSameCoefficientRule::checkRule(FDataCardStruct& dataStruct)
{
	return false;
}

bool UChooseSameCharacterNameRule::checkRule(FDataCardStruct& dataStruct)
{
	return false;
}

bool getStatusTask()
{
	return false;
}

FString getDescriptionTask()
{
	return FString();
}

bool UTask::getStatusTask()
{
	return false;
}

FString UTask::getDescriptionTask()
{
	return FString();
}

FString UTaskUserWidget::getDescriptionTask()
{
	return FString("AAAAAAAAAAAAAA");
}

bool UTaskUserWidget::getStatusTask()
{
	return true;
}
