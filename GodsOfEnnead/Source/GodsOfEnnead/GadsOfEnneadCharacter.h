// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "GadsOfEnneadCharacter.generated.h"

UCLASS()
class GODSOFENNEAD_API AGadsOfEnneadCharacter : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float InteractLineTraceLength = 1000.0f;

public:
	// Sets default values for this character's properties
	AGadsOfEnneadCharacter();
	void TakeCard(ACardActor* card);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	USpringArmComponent* CameraBoom;
	UCameraComponent* FollowCamera;
};
