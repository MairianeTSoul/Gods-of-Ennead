// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <TaskUserWidget.h>
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
	void init();
	UFUNCTION(BlueprintCallable)
	void tick(float dt);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
	TSubclassOf<UUserWidget> widgetClass;

	//TODO Where we would store tasks?
	UPROPERTY()
	TObjectPtr<UTask> m_task;
	UPROPERTY()
	TObjectPtr<UUserWidget> m_widget;
};
