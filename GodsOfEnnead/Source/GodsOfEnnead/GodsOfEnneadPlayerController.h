// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TaskController.h"
#include "TurnStatus.h"
#include "GameFramework/PlayerController.h"
#include "Cards/Hand.h"
#include "GodsOfEnneadPlayerController.generated.h"
class ACardActor;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnimationFinished);

const uint32 g_cardCount = 108;

const FRotator SHOW_ROTATION = FRotator(90.0f, 19.471221f, -160.528779f);
const FRotator HIDE_ROTATION = FRotator(-90.0f, 19.471221f, -160.528779f);


UCLASS()
class GODSOFENNEAD_API AGodsOfEnneadPlayerController : public APlayerController
{
	GENERATED_BODY()
	AGodsOfEnneadPlayerController();

public:
	
	virtual void BeginPlay() override;

	void TakeCard();
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MovePlayerToTarget();

	bool bGameOver = false;
	ETurnStatus CurrentTurnStatus = ETurnStatus::Waiting;

	void StartGame();
	void PlayRound();

	void ComputerTurn();
	void DiscardUnnecessaryCard();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cards")
	TArray<UHand*> PlayersHands;

	void DealCards(int32 NumCards, bool bIsPlayer);
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAnimationFinished OnAnimationFinished;

	UFUNCTION(BlueprintCallable, Category = "CardsController")
	void SpawnActors();

	TArray<ACardActor*> AllCardsActors;
	TArray<ACardActor*> DeckCardsActors;
	TArray<ACardActor*> ShowDeckCardsActors;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cards")
	ACardActor* SelectedCardActor;

	virtual void SetupInputComponent() override;

	// Task
	UPROPERTY()
	TObjectPtr<UTaskController> CurrentTaskController;

	UFUNCTION()
	void DisplayTask();

private:
	void UpdateMovement(float DeltaTime);
	void Tick(float DeltaTime);

	TObjectPtr<UDataTable> CardDataTable;

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

	int32 CardsInHand = 0;

	void FixDeck(TArray<ACardActor*> Deck);

	void SpawnActorStep(const FVector& StartLocation, const FRotator& StartRotation);
};
