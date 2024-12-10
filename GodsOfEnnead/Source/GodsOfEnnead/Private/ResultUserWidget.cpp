// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultUserWidget.h"

void UResultUserWidget::SetResultText(bool bIsWin)
{
	if (!ResultText) return;

	if (bIsWin)
	{
		ResultText->SetText(FText::FromString("WIN"));
		ResultText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
	}
	else
	{
		ResultText->SetText(FText::FromString("GAME OVER"));
		ResultText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	}
}