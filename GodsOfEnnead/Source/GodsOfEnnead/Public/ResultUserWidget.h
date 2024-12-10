// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ResultUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class GODSOFENNEAD_API UResultUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResultText;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetResultText(bool bIsWin);
};
