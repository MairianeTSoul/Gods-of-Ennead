// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultUserWidget.h"

void UResultUserWidget::SetResultText(bool bIsWin)
{
	if (!ResultText) return;

	if (bIsWin)
	{
		ResultText->SetText(FText::FromString("YOU WIN"));
		ResultText->SetColorAndOpacity(FSlateColor(FLinearColor(0.020833, 0.012454, 0.007127, 1)));
	}
	else
	{
		ResultText->SetText(FText::FromString("GAME OVER"));
		ResultText->SetColorAndOpacity(FSlateColor(FLinearColor(0.020833, 0.012454, 0.007127, 1)));
	}
}