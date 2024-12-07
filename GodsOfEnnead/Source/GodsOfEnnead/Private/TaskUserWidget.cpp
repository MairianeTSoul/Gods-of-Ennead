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
		return FString::Printf(TEXT("Choose card with attack value %d"), valueRule);
	}
	return FString::Printf(TEXT("Choose card with hp value %d"), valueRule);
	
}

bool UChooseSameCharacterNameRule::checkRule(FDataCardStruct& dataStruct)
{
	return dataStruct.cardName == valueRule;
}

FString UChooseSameCharacterNameRule::getNameRule()
{
	return "Choose card with name character " + valueRule;
}

bool getStatusTask()
{
	return false;
}

FString getDescriptionTask()
{
	return FString();
}

void UTask::updateState(TArray<FDataCardStruct>& cards)
{
	isComplete = true;
	for (int i = 0; i < cards.Num(); i++)
	{
		isComplete = isComplete && (rules[i].GetDefaultObject())->checkRule(cards[i]);
	}
}

bool UTask::getStatusTask()
{
	return isComplete;
}

FString UTask::getDescriptionTask()
{
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

void UTaskUserWidget::setTask(UTask* inTask)
{
	task = inTask;
}
