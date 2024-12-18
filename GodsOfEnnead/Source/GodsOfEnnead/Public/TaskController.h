// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <TaskUserWidget.h>

#include "Task.h"
#include "TaskController.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GODSOFENNEAD_API UTaskController : public UObject
{
	GENERATED_BODY()

public:
	UTaskController();
	~UTaskController();

	UFUNCTION(BlueprintCallable)
	void init(UWorld* World);

	UFUNCTION()
	void GenerateTask();

	UPROPERTY()
	TObjectPtr<UTask> Task;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
	TSubclassOf<UTaskUserWidget> widgetClass;
	
	UPROPERTY()
	TObjectPtr<UTaskUserWidget> TaskWidget;
};
