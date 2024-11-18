// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styling/SlateBrush.h"
#include "CardUserWidget.generated.h"


/**
 * 
 */
UCLASS(Blueprintable)
class GODSOFENNEAD_API UCardUserWidget : public UUserWidget
{
	GENERATED_BODY()



	public:
	UPROPERTY(EditAnywhere, Category = "Stats")
	int new_attack;
	UPROPERTY(EditAnywhere, Category = "Stats")
	int new_hp;
	UPROPERTY(EditAnywhere, Category = "Stats")
	FString new_name_character;
	UPROPERTY(EditAnywhere, Category = "Stats")
	FSlateBrush new_icon_character;

};
