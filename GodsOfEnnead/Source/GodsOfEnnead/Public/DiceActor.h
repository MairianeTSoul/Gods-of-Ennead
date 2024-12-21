// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "DiceActor.generated.h"

UENUM(BlueprintType)
enum class EDiceResult : uint8
{
	Black UMETA(DisplayName = "Black"),
	Grey UMETA(DisplayName = "Grey"),
	White UMETA(DisplayName = "White"),
	None UMETA(DisplayName = "None")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDiceStop);

UCLASS()
class GODSOFENNEAD_API ADiceActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADiceActor();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DiceMesh;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* Arrow1;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* Arrow2;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* Arrow3;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* Arrow4;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* Arrow5;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* Arrow6;

	UPROPERTY(VisibleAnywhere)
	UPhysicsHandleComponent* PhysicsHandle;

	FVector StartLocation;

	
	FVector StartPosition;
	FRotator StartRotation;
	FVector TargetPosition;
	FRotator TargetRotation;
	float ElapsedTime;

	const float AnimationDuration = 0.8f;
	bool bIsAnimating;

public:
	UPROPERTY(BlueprintAssignable, Category = "Dice")
	FOnDiceStop OnDiceStop;
	
	void RollDice();

	void DropDice();

	EDiceResult GetDiceResult() const;
	void AnimateTo(const FVector* FinalPos, const FRotator* FinalRot);

	EDiceResult DiceResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TArray<UMaterialInterface*> Materials;
	
	virtual void Tick(float DeltaTime) override;
};
