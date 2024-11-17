// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GodsOfEnneadPlayerController.generated.h"
class ACardActor;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnimationFinished);

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

private:
	void UpdateMovement(float DeltaTime);
	void Tick(float DeltaTime);

	FVector StartLocation; // Начальная позиция игрока
	FRotator StartRotation; // Начальная позиция игрока
	FVector EndLocation;   // Конечная позиция игрока
	FRotator EndRotation;   // Конечная позиция игрока
	float MoveDuration;    // Время перемещения
	float ElapsedTime;     // Время, прошедшее с начала движения
	bool bIsMoving;        // Флаг, указывающий на движение

	FTimerHandle SpawnTimerHandle;
	int32 SpawnedActorCount = 0;

	void SpawnActorStep(const FVector& StartLocation, const FRotator& StartRotation);
};
