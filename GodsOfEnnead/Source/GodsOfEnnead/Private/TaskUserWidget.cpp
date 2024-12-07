// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskUserWidget.h"

bool URule::checkRule(FDataCardStruct& dataStruct)
{
	return false;
}

FString URule::getNameRule()
{
	return nameRule;
}


//////////////////////////////////////////
bool UChooseSameCoefficientRule::checkRule(FDataCardStruct& dataStruct)
{
	if (isAttackValue)
	{
		return dataStruct.attack == valueRule;
	}
	return dataStruct.hp == valueRule;
}

FString UChooseSameCoefficientRule::getNameRule()
{
	if (isAttackValue)
	{
		return FString::Printf(TEXT("Выберите карту со значением аттаки %d"), valueRule);
	}
	return FString::Printf(TEXT("Выберите карту со значением hp %d"), valueRule);
	
}

bool UChooseSameCharacterNameRule::checkRule(FDataCardStruct& dataStruct)
{
	return dataStruct.cardName == valueRule;
}

FString UChooseSameCharacterNameRule::getNameRule()
{
	return "Выберите карту с именем героя " + valueRule;
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
	bool statusTasks = false;
	if (rules.Num() != cardsStruct.Num())
	{
		return false;
	}

	for (int i = 0; i < cardsStruct.Num(); i++)
	{
		statusTasks = statusTasks && (rules[i].GetDefaultObject())->checkRule(cardsStruct[i]);
	}
	return statusTasks;
}

FString UTask::getDescriptionTask()
{
	if (rules.Num() != cardsStruct.Num()) 
	{
		return FString("Task not found");
	}
	FString description = "";
	for (int i = 0; i < rules.Num(); i++)
	{
		description += (rules[i].GetDefaultObject())->getNameRule() + "\n";
	}
	return description;
}

FString UTaskUserWidget::getDescriptionTask()
{
	if (!task)
	{
		return "";
	}

	return task->getDescriptionTask();
}

bool UTaskUserWidget::getStatusTask()
{
	if (!task)
	{
		return false;
	}

	return task->getStatusTask();
}
