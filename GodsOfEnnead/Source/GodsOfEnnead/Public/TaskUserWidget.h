// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <CardActor.h>
#include "TaskUserWidget.generated.h"

UCLASS(Blueprintable)
class GODSOFENNEAD_API URule : public UObject
{
	GENERATED_BODY()
	public:
		UPROPERTY(EditAnywhere, Category = "TasksRule")
		FString nameRule;
		UFUNCTION()
		virtual bool checkRule(struct FDataCardStruct& dataStruct);
		UFUNCTION()
		virtual FString getNameRule();
};

UCLASS(Blueprintable)
class GODSOFENNEAD_API UChooseSameCoefficientRule : public URule
{
	GENERATED_BODY()
	public:
		UPROPERTY(EditAnywhere, Category = "TasksRule")
		int valueRule;
		UPROPERTY(EditAnywhere, Category = "TasksRule")
		bool isAttackValue;
		UChooseSameCoefficientRule() {
		};
		UChooseSameCoefficientRule(int value) {
			valueRule = value;
		};
		bool checkRule(struct FDataCardStruct& dataStruct);
		FString getNameRule();
};

UCLASS(Blueprintable)
class GODSOFENNEAD_API UChooseSameCharacterNameRule : public URule
{
	GENERATED_BODY()
	public:
		UPROPERTY(EditAnywhere, Category = "TasksRule")
		FString valueRule;

		UChooseSameCharacterNameRule()
		{};
		UChooseSameCharacterNameRule(FString value) {
			valueRule = value;
		};

		bool checkRule(struct FDataCardStruct& dataStruct);
		FString getNameRule();
};

UCLASS(Blueprintable)
class GODSOFENNEAD_API UTask : public UObject
{
	GENERATED_BODY()
	public:
	UPROPERTY(EditAnywhere, Category = "Task")
	TArray<TSubclassOf<URule>> rules;
	UPROPERTY()
	bool isComplete;

	UFUNCTION()
	void updateState(TArray<FDataCardStruct>& cards);

	UFUNCTION()
	virtual bool getStatusTask();
	UFUNCTION()
	virtual FString getDescriptionTask();
};

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

UCLASS()
class GODSOFENNEAD_API UTaskUserWidget : public UUserWidget
{
	GENERATED_BODY()
	public:
	UPROPERTY()
	TObjectPtr<UTask> task;
	UFUNCTION(BlueprintCallable, Category = "Task")
	FString getDescriptionTask();

	UFUNCTION(BlueprintCallable, Category = "Task")
	bool getStatusTask();

	UFUNCTION()
	void setTask(UTask* inTask);

};
