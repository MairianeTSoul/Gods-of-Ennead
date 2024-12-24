// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <CardActor.h>

#include "Task.h"
#include "Components/Button.h"
#include "TaskUserWidget.generated.h"

// UCLASS(Blueprintable)
// class GODSOFENNEAD_API URule : public UObject
// {
// 	GENERATED_BODY()
// 	public:
// 		UPROPERTY(EditAnywhere, Category = "TasksRule")
// 		FString nameRule;
// 		UFUNCTION()
// 		virtual bool checkRule(ACardActor& Card);
// 		UFUNCTION()
// 		virtual FString getNameRule();
// };
//
// UCLASS(Blueprintable)
// class GODSOFENNEAD_API UChooseSameCoefficientRule : public URule
// {
// 	GENERATED_BODY()
// 	public:
// 		UPROPERTY(EditAnywhere, Category = "TasksRule")
// 		int valueRule;
// 		UPROPERTY(EditAnywhere, Category = "TasksRule")
// 		bool isAttackValue;
// 		UChooseSameCoefficientRule() {
// 		};
// 		UChooseSameCoefficientRule(int value) {
// 			valueRule = value;
// 		};
// 		bool checkRule(ACardActor& Card);
// 		FString getNameRule();
// };
//
// UCLASS(Blueprintable)
// class GODSOFENNEAD_API UChooseSameCharacterNameRule : public URule
// {
// 	GENERATED_BODY()
// 	public:
// 		UPROPERTY(EditAnywhere, Category = "TasksRule")
// 		FString valueRule;
//
// 		UChooseSameCharacterNameRule()
// 		{};
// 		UChooseSameCharacterNameRule(FString value) {
// 			valueRule = value;
// 		};
//
// 		bool checkRule(ACardActor& Card);
// 		FString getNameRule();
// };
//
// UCLASS(Blueprintable)
// class GODSOFENNEAD_API UTask : public UObject
// {
// 	GENERATED_BODY()
// 	public:
// 	UPROPERTY(EditAnywhere, Category = "Task")
// 	TArray<TSubclassOf<URule>> rules;
// 	UPROPERTY()
// 	bool isComplete;
//
// 	UFUNCTION()
// 	void updateState(TArray<ACardActor>& cards);
//
// 	UFUNCTION()
// 	virtual bool getStatusTask();
// 	UFUNCTION()
// 	virtual FString getDescriptionTask();
// };

//UCLASS()
//class GODSOFENNEAD_API UTaskArifmeticProgressiveValues : public UTask
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, Category = "Task")
//	TArray<TSubclassOf<URule>> rules;
//
//	UTask()
//	{};
//	UTask(TArray<TSubclassOf<URule>> new_rules) {
//		rules = new_rules;
//	};
//
//	bool checkStatusTask();
//	FString getDescriptionTask();
//};
UENUM(BlueprintType)
enum class ETurnStatus : uint8
{
	Waiting					UMETA(DisplayName = "Waiting"),
	Waiting_Choose			UMETA(DisplayName = "Waiting Choose"),
	Player_Turn				UMETA(DisplayName = "Player Turn"),
	Computer_Turn			UMETA(DisplayName = "Computer Turn"),
	Second_Round_Start		UMETA(DisplayName = "Second Round Start"),
	Second_Round_Player		UMETA(DisplayName = "Second Round Player Turn"),
	Second_Round_Waiting	UMETA(DisplayName = "Second Round Waiting"),
	Second_Round_Dice		UMETA(DisplayName = "Second Round Dice Waiting"),
};

UCLASS()
class GODSOFENNEAD_API UTaskUserWidget : public UUserWidget
{
	GENERATED_BODY()
	UFUNCTION(BlueprintCallable, Category = "Task")
	void OnReadyButtonClicked();
	public:
	UPROPERTY()
	TObjectPtr<UTask> Task;
	UFUNCTION(BlueprintCallable, Category = "Task")
	FString getDescriptionTask();

	UFUNCTION(BlueprintCallable, Category = "Task")
	bool getStatusTask();
	UFUNCTION(BlueprintCallable, Category = "Task")
	bool getComputerStatusTask();

	UFUNCTION(BlueprintCallable, Category = "Task")
	bool setButtonVisibility();

	UFUNCTION(BlueprintCallable, Category = "Task")
	FText GetDiceResult();

	UFUNCTION(BlueprintCallable, Category = "Task")
	FText GetHintText();

	UPROPERTY()
	EDiceResult DiceResult = EDiceResult::None;

	UPROPERTY()
	ETurnStatus TurnStatus = ETurnStatus::Waiting;
	
	UFUNCTION()
	void setTask(UTask* NewTask);
	

	bool bVisibility = false;
};
