﻿#include "DiceActor.h"

#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

#include "Kismet/GameplayStatics.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

// Конструктор
ADiceActor::ADiceActor()
{
	PrimaryActorTick.bCanEverTick = true;

	DiceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DiceMesh"));
	RootComponent = DiceMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Game/Assets/StaticMeshes/cube.cube"));
	if (CubeMesh.Succeeded())
	{
		DiceMesh->SetStaticMesh(CubeMesh.Object);
		DiceMesh->SetRelativeScale3D(FVector(60, 60, 60)); 
	}

	Arrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow1"));
	Arrow1->SetupAttachment(DiceMesh);
	Arrow1->SetRelativeLocation(FVector(0, 0, 50));  // Сторона 1 (верх)
	Arrow1->SetRelativeRotation(FRotator(0, 0, 0));  // Направлено вверх

	Arrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow2"));
	Arrow2->SetupAttachment(DiceMesh);
	Arrow2->SetRelativeLocation(FVector(0, 0, -50)); // Сторона 2 (низ)
	Arrow2->SetRelativeRotation(FRotator(180, 0, 0));  // Направлено вниз

	Arrow3 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow3"));
	Arrow3->SetupAttachment(DiceMesh);
	Arrow3->SetRelativeLocation(FVector(50, 0, 0));  // Сторона 3 (правая)
	Arrow3->SetRelativeRotation(FRotator(0, 90, 0));  // Направлено вправо

	Arrow4 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow4"));
	Arrow4->SetupAttachment(DiceMesh);
	Arrow4->SetRelativeLocation(FVector(-50, 0, 0)); // Сторона 4 (левая)
	Arrow4->SetRelativeRotation(FRotator(0, -90, 0));  // Направлено влево

	Arrow5 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow5"));
	Arrow5->SetupAttachment(DiceMesh);
	Arrow5->SetRelativeLocation(FVector(0, 50, 0));  // Сторона 5 (передняя)
	Arrow5->SetRelativeRotation(FRotator(90, 0, 0));  // Направлено вперед

	Arrow6 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow6"));
	Arrow6->SetupAttachment(DiceMesh);
	Arrow6->SetRelativeLocation(FVector(0, -50, 0)); // Сторона 6 (задняя)
	Arrow6->SetRelativeRotation(FRotator(-90, 0, 0));  // Направлено назад
}

void ADiceActor::BeginPlay()
{
	Super::BeginPlay();
    
	if (DiceMesh)
	{
		DiceMesh->SetSimulatePhysics(true);
		DiceMesh->SetMassOverrideInKg(NAME_None, 1.0f);
		DiceMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DiceMesh->SetCollisionResponseToAllChannels(ECR_Block);

		PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

		StartLocation = FVector(8177.0f, 8380.0f, 4000.0f);
	}
}

void ADiceActor::RollDice()
{
	FVector TargetLocation = GetActorLocation() + FVector(0, 0, 750);
	TargetLocation.X = StartLocation.X;
	TargetLocation.Y = StartLocation.Y;	
	const FRotator RandomRotation = FRotator(FMath::RandRange(0.f, 360.f), FMath::RandRange(0.f, 360.f), FMath::RandRange(0.f, 360.f));
	AnimateTo(&TargetLocation, &RandomRotation);

	DiceResult = EDiceResult::None;
}

void ADiceActor::DropDice()
{
	// FVector GravityForce = FVector(0, 0, -10.0f);
	// DiceMesh->AddForce(GravityForce);
}

EDiceResult ADiceActor::GetDiceResult() const
{
	const FVector UpDirection = FVector(0, 0, 50);

	const float Tolerance = 0.05f;

	if (FVector::DotProduct(Arrow1->GetComponentRotation().Vector(), UpDirection) > 1.0f - Tolerance)
	{
		return EDiceResult::Black;
	}
	if (FVector::DotProduct(Arrow2->GetComponentRotation().Vector(), UpDirection) > 1.0f - Tolerance)
	{
		return EDiceResult::Black;
	}
	if (FVector::DotProduct(Arrow3->GetComponentRotation().Vector(), UpDirection) > 1.0f - Tolerance)
	{
		return EDiceResult::Grey;
	}
	if (FVector::DotProduct(Arrow4->GetComponentRotation().Vector(), UpDirection) > 1.0f - Tolerance)
	{
		return EDiceResult::Grey;
	}
	if (FVector::DotProduct(Arrow5->GetComponentRotation().Vector(), UpDirection) > 1.0f - Tolerance)
	{
		return EDiceResult::White;
	}
	if (FVector::DotProduct(Arrow6->GetComponentRotation().Vector(), UpDirection) > 1.0f - Tolerance)
	{
		return EDiceResult::White;
	}

	return EDiceResult::None;
}

void ADiceActor::AnimateTo(const FVector* FinalPos, const FRotator* FinalRot)
{
	bIsAnimating = true;

	StartPosition = GetActorLocation();
	StartRotation = GetActorRotation();
	TargetPosition = StartPosition;
	TargetRotation = StartRotation;
	
	if (FinalPos)
	{
		TargetPosition = *FinalPos;
	}

	if (FinalRot)
	{
		TargetRotation = *FinalRot;
	}

	ElapsedTime = 0.0f;
}


void ADiceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Velocity = DiceMesh->GetPhysicsLinearVelocity();
	if (!bIsAnimating && GetActorLocation().Z <= 1761 && Velocity.SizeSquared() <= 0.3f)
	{
		DiceResult = GetDiceResult();

		OnDiceStop.Broadcast();
	}
	else
	{
		DiceResult = EDiceResult::None;
	}
	
	if (bIsAnimating)
	{
		ElapsedTime += DeltaTime;
		const float Alpha = FMath::Clamp(ElapsedTime / AnimationDuration, 0.0f, 1.0f);

		const float SmoothAlpha = FMath::InterpEaseOut(0.0f, 1.0f, Alpha, 2.0f);

		const FVector NewPosition = FMath::Lerp(StartPosition, TargetPosition, SmoothAlpha);
		const FRotator NewRotation = FMath::Lerp(StartRotation, TargetRotation, SmoothAlpha);
		SetActorLocationAndRotation(NewPosition, NewRotation);

		if (Alpha >= 1.0f)
		{
			bIsAnimating = false;
			ElapsedTime = 0.0f;
			DropDice();
		}
	}
}
