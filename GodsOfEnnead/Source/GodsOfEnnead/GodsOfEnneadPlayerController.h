// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GodsOfEnneadPlayerController.generated.h"
class ACardActor;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnimationFinished);

const uint32 g_cardCount = 108;
const uint32 g_cardTypes = 3;

USTRUCT()
struct FSomeDataStruct
{
	GENERATED_BODY()
	int a;
	int b;
	FString str;
};


UCLASS()
class GODSOFENNEAD_API AGodsOfEnneadPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MovePlayerToTarget();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAnimationFinished OnAnimationFinished;

	UFUNCTION(BlueprintCallable, Category = "CardsController")
	void SpawnActors();

	TArray<ACardActor*> SpawnedActors;
	virtual void SetupInputComponent() override;
	void HandleClick();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cards")
	ACardActor* SelectedCardActor;

	void SelectCard(ACardActor* CardActor);

private:
	void UpdateMovement(float DeltaTime);
	void Tick(float DeltaTime);

	UPROPERTY()
	FVector StartLocation; // Начальная позиция игрока
	UPROPERTY()
	FRotator StartRotation; // Начальная позиция игрока
	UPROPERTY()
	FVector EndLocation;   // Конечная позиция игрока
	UPROPERTY()
	FRotator EndRotation;   // Конечная позиция игрока
	UPROPERTY()
	float MoveDuration;    // Время перемещения
	UPROPERTY()
	float ElapsedTime;     // Время, прошедшее с начала движения
	UPROPERTY()
	bool bIsMoving;        // Флаг, указывающий на движение

	UPROPERTY()
	FTimerHandle SpawnTimerHandle;
	UPROPERTY()
	int32 SpawnedActorCount = 0;

	//TODO Use TArray instead of raw arrays
	UPROPERTY()
	FSomeDataStruct DataVal[g_cardTypes]{ {2, 2, "Bastet"}, {5, 7, "Ra"}, {6, 9, "Test"}};

	void SpawnActorStep(const FVector& StartLocation, const FRotator& StartRotation);
};
