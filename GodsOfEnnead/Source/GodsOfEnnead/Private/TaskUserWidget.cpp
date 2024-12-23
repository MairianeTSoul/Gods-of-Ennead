// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskUserWidget.h"


#include "GodsOfEnnead/GodsOfEnneadPlayerController.h"

// bool URule::checkRule(ACardActor& Card)
// {
// 	return false;
// }
//
// FString URule::getNameRule()
// {
// 	return nameRule;
// }
//
//
// //////////////////////////////////////////
// bool UChooseSameCoefficientRule::checkRule(ACardActor& Card)
// {
// 	if (isAttackValue)
// 	{
// 		return Card.GetDataCard().attack == valueRule;
// 	}
// 	return Card.CardsData.hp == valueRule;
// }
//
// FString UChooseSameCoefficientRule::getNameRule()
// {
// 	if (isAttackValue)
// 	{
// 		return FString::Printf(TEXT("Choose card with attack value %d"), valueRule);
// 	}
// 	return FString::Printf(TEXT("Choose card with hp value %d"), valueRule);
// 	
// }
//
// bool UChooseSameCharacterNameRule::checkRule(ACardActor& Card)
// {
// 	return Card.GetDataCard().cardName == valueRule;
// }
//
// FString UChooseSameCharacterNameRule::getNameRule()
// {
// 	return "Choose card with name character " + valueRule;
// }
//
// bool getStatusTask()
// {
// 	return false;
// }
//
// FString getDescriptionTask()
// {
// 	return FString();
// }
//
// void UTask::updateState(TArray<ACardActor>& cards)
// {
// 	isComplete = true;
// 	for (int i = 0; i < cards.Num(); i++)
// 	{
// 		isComplete = isComplete && (rules[i].GetDefaultObject())->checkRule(cards[i]);
// 	}
// }
//
// bool UTask::getStatusTask()
// {
// 	return isComplete;
// }
//
// FString UTask::getDescriptionTask()
// {
// 	FString description = "";
// 	for (int i = 0; i < rules.Num(); i++)
// 	{
// 		description += (rules[i].GetDefaultObject())->getNameRule() + "\n";
// 	}
// 	return description;
// }

void UTaskUserWidget::OnReadyButtonClicked()
{
	if (AGodsOfEnneadPlayerController* PlayerController = Cast<AGodsOfEnneadPlayerController>(GetOwningPlayer()))
	{
		PlayerController->OnReadyButtonClicked();
		bVisibility = false;
	}
}

FString UTaskUserWidget::getDescriptionTask()
{
	if (!Task)
	{
		return "";
	}
	return Task->getDescriptionTask();
}

bool UTaskUserWidget::getStatusTask()
{
	if (!Task)
	{
		return false;
	}
	
	return Task->GetStatusTask();
}

bool UTaskUserWidget::getComputerStatusTask()
{
		if (!Task)
    	{
    		return false;
    	}
    	
    	return Task->GetComputerStatusTask();
}


void UTaskUserWidget::setTask(UTask* NewTask)
{
	Task = NewTask;
}

bool UTaskUserWidget::setButtonVisibility()
{
	return bVisibility;
}

FText UTaskUserWidget::GetDiceResult()
{
	if (DiceResult == EDiceResult::White)
		return FText::FromString(UTF8TEXT("White: Full-strength attack"));
	if (DiceResult == EDiceResult::Grey)
		return FText::FromString(UTF8TEXT("Grey: Half-strength attack"));
	if (DiceResult == EDiceResult::Black)
		return FText::FromString(UTF8TEXT("Black: Attack failed"));
	return FText::FromString("");
}

FText UTaskUserWidget::GetHintText()
{
	if (AGodsOfEnneadPlayerController* PlayerController = Cast<AGodsOfEnneadPlayerController>(GetOwningPlayer()))
	{
		TurnStatus = PlayerController->CurrentTurnStatus;
		if (TurnStatus == ETurnStatus::Waiting)
			return FText::FromString(UTF8TEXT("Waiting..."));
		if (TurnStatus == ETurnStatus::Waiting_Choose)
			return FText::FromString(UTF8TEXT("Select card from one of decks"));
		if (TurnStatus == ETurnStatus::Player_Turn)
			return FText::FromString(UTF8TEXT("Remove unsuitable card"));
		if (TurnStatus == ETurnStatus::Computer_Turn)
			return FText::FromString(UTF8TEXT("Wait for opponent's move"));
		if (TurnStatus == ETurnStatus::Second_Round_Start)
			return FText::FromString(UTF8TEXT("The battle begins!"));
		if (TurnStatus == ETurnStatus::Second_Round_Player)
			return FText::FromString(UTF8TEXT("Click on cards to swap  them"));
		if (TurnStatus == ETurnStatus::Second_Round_Waiting)
			return FText::FromString(UTF8TEXT("Opponent's move"));
		if (TurnStatus == ETurnStatus::Second_Round_Dice)
			return FText::FromString(UTF8TEXT("Roll dice to empower card"));
	}
	return FText::FromString("");
}