// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiceActor.h"
#include "TaskController.h"
#include "TurnStatus.h"
#include "GameFramework/PlayerController.h"
#include "Cards/Hand.h"
#include "GodsOfEnneadPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnimationFinished);

constexpr uint32 GCardCount = 108;

const FRotator GShow_Rotation = FRotator(90.0f, 0, 180);
const FRotator GHide_Rotation = FRotator(-90.0f, 0, 0);

const FVector GShow_Location = FVector(8177.0f, 7680.0f, 2000.0f);
const FVector GHide_Location = FVector(8177.00f, 6755.00f, 1800.00f);
const FVector GComputer_Card_Location = FVector(8918.0f, 5571.0f, 2216.0f);
const FVector GPlayer_Card_Location = FVector(7550.0f, 5571.0f, 2216.0f);

const FVector GCard_Scale = FVector(90.0f, 19.471221f, 180.528779f);

const FVector GCamera_Position = FVector(7881.0f, 7166.00f, 4084.00f); // 8141
const FRotator GCamera_Rotation = FRotator(-77.0f, 0.0f, 0.0f); // -85


UCLASS()
class GODSOFENNEAD_API AGodsOfEnneadPlayerController : public APlayerController
{
	GENERATED_BODY()
	AGodsOfEnneadPlayerController();

public:
	
	virtual void BeginPlay() override;

	void TakeCard();
	UFUNCTION()
	void OnDiceStopped();
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MovePlayerToTarget();

	bool bIsPlayerWin = false;
	bool bIsGameOver = false;
	int CurrentAttackerIndex;

	void ProcessAttack(const UWorld* World, UHand* AttackerHand, UHand* DefenderHand, int32 Index, bool bIsPlayerAttacker, EDiceResult
	                   DiceResult);
	UFUNCTION()
	void OnDiceComputerStopped();
	void AddResultToViewPort();
	
	ETurnStatus CurrentTurnStatus = ETurnStatus::Waiting;

	void StartGame();
	void PlayRound();
	void StartSecondTour();
	void RemoveDeckCads();

	void ComputerTurn();
	void DiscardUnnecessaryCard();

	
	UPROPERTY()
	ACardActor* SelectedCard;
	
	UPROPERTY()
	ADiceActor* DiceActor;

	void SelectCard(ACardActor* Card);

	void SwapCards(ACardActor* Card1, ACardActor* Card2);
	void OnReadyButtonClicked();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cards")
	TArray<UHand*> PlayersHands;

	void DealCards(int32 NumCards, bool bIsPlayer);
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAnimationFinished OnAnimationFinished;

	UFUNCTION(BlueprintCallable, Category = "CardsController")
	void SpawnActors();

	TArray<ACardActor*> DeckCardsActors;
	TArray<ACardActor*> ShowDeckCardsActors;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cards")
	ACardActor* SelectedCardActor;

	virtual void SetupInputComponent() override;
	void ExitToMainMenu();

	// Task
	UPROPERTY()
	TObjectPtr<UTaskController> CurrentTaskController;

	UFUNCTION()
	void DisplayTask();

private:
	void UpdateMovement(float DeltaTime);
	void Tick(float DeltaTime);

	TArray<FDataCardStruct*> dtRows;

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
