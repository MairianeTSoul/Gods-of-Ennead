// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Task.generated.h"


UENUM(BlueprintType)
enum class ETaskPattern : uint8
{
	HP     UMETA(DisplayName = "HP value"),
	ATTACK UMETA(DisplayName = "HA value"),
	NAME   UMETA(DisplayName = "Name")
};

UENUM(BlueprintType)
enum class ETaskType : uint8
{
	RUN UMETA(DisplayName = "Run"),
	SET UMETA(DisplayName = "Set")
};

/**
 * 
 */
UCLASS()
class GODSOFENNEAD_API UTask : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	ETaskPattern Pattern;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	int32 Count;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	ETaskType Type;

	UPROPERTY()
	bool bIsComplete = false;
	bool bIsComputerComplete = false;

	UTask();

	UFUNCTION(BlueprintCallable, Category = "Task")
	void InitializeTask(ETaskPattern InPattern, int32 InCount, ETaskType InType);

	UFUNCTION(BlueprintCallable, Category = "Task")
	bool CheckTaskCompletion(const TArray<FDataCardStruct>& Cards, bool bIsCompleteStatusSet);

	UFUNCTION(BlueprintCallable, Category = "Task")
	int32 GetClosestCountToCompletion(const TArray<FDataCardStruct>& Cards);
	
	bool GetStatusTask();
	bool GetComputerStatusTask();
	FString getDescriptionTask();
};
