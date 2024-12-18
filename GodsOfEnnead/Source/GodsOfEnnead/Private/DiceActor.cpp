#include "DiceActor.h"

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

	// Устанавливаем стандартную сетку куба (размер 40x40)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (CubeMesh.Succeeded())
	{
		DiceMesh->SetStaticMesh(CubeMesh.Object);
		DiceMesh->SetRelativeScale3D(FVector(1.4f, 1.4f, 1.4f)); 
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

	// Настройка физических свойств кубика
	DiceMesh->SetSimulatePhysics(true);
	DiceMesh->SetMassOverrideInKg(NAME_None, 1.0f);
	DiceMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DiceMesh->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BlackMaterial(TEXT("/Game/Materials/M_Black.M_Black"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GreyMaterial(TEXT("/Game/Materials/M_Grey.M_Grey"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> WhiteMaterial(TEXT("/Game/Materials/M_White.M_White"));

	if (BlackMaterial.Succeeded())
	{
		Materials.Add(BlackMaterial.Object);
	}
	if (GreyMaterial.Succeeded())
	{
		Materials.Add(GreyMaterial.Object);
	}
	if (WhiteMaterial.Succeeded())
	{
		Materials.Add(WhiteMaterial.Object);
	}

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	StartLocation = FVector(8177.0f, 8380.0f, 4000.0f);
}

void ADiceActor::ApplyMaterials()
{
	if (Materials.Num() >= 3)
	{
		DiceMesh->SetMaterial(0, Materials[0]);  // Сторона 1 (черная)
		DiceMesh->SetMaterial(1, Materials[1]);  // Сторона 2 (серая)
		DiceMesh->SetMaterial(2, Materials[2]);  // Сторона 3 (белая)
		DiceMesh->SetMaterial(3, Materials[0]);  // Сторона 4 (черная)
		DiceMesh->SetMaterial(4, Materials[1]);  // Сторона 5 (серая)
		DiceMesh->SetMaterial(5, Materials[2]);  // Сторона 6 (белая)
	}
}

void ADiceActor::BeginPlay()
{
	Super::BeginPlay();
}

void ADiceActor::RollDice()
{
	const FVector TargetLocation = StartLocation + FVector(0, 0, 10);
	const FRotator RandomRotation = FRotator(FMath::RandRange(0.f, 360.f), FMath::RandRange(0.f, 360.f), FMath::RandRange(0.f, 360.f));
	AnimateTo(&TargetLocation, &RandomRotation);

	DiceResult = EDiceResult::None;
}

void ADiceActor::DropDice()
{
	FVector GravityForce = FVector(0, 0, -5000.0f);
	DiceMesh->AddForce(GravityForce);
}

EDiceResult ADiceActor::GetDiceResult() const
{
	const FVector UpDirection = FVector(0, 0, 50);

	const float Tolerance = 0.01f;

	if (FVector::DotProduct(Arrow1->GetComponentRotation().Vector(), UpDirection) > 1.0f - Tolerance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Arrow1 points upwards, Result: Black"));
		return EDiceResult::Black;
	}
	if (FVector::DotProduct(Arrow4->GetComponentRotation().Vector(), UpDirection) > 1.0f - Tolerance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Arrow4 points upwards, Result: Black"));
		return EDiceResult::Black;
	}
	if (FVector::DotProduct(Arrow2->GetComponentRotation().Vector(), UpDirection) > 1.0f - Tolerance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Arrow2 points upwards, Result: Grey"));
		return EDiceResult::Grey;
	}
	if (FVector::DotProduct(Arrow5->GetComponentRotation().Vector(), UpDirection) > 1.0f - Tolerance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Arrow5 points upwards, Result: Grey"));
		return EDiceResult::Grey;
	}
	if (FVector::DotProduct(Arrow3->GetComponentRotation().Vector(), UpDirection) > 1.0f - Tolerance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Arrow3 points upwards, Result: White"));
		return EDiceResult::White;
	}
	if (FVector::DotProduct(Arrow6->GetComponentRotation().Vector(), UpDirection) > 1.0f - Tolerance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Arrow6 points upwards, Result: White"));
		return EDiceResult::White;
	}

	UE_LOG(LogTemp, Warning, TEXT("Arrow1 Direction: %s"), *Arrow1->GetComponentRotation().Vector().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Arrow2 Direction: %s"), *Arrow2->GetComponentRotation().Vector().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Arrow3 Direction: %s"), *Arrow3->GetComponentRotation().Vector().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Arrow4 Direction: %s"), *Arrow4->GetComponentRotation().Vector().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Arrow5 Direction: %s"), *Arrow5->GetComponentRotation().Vector().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Arrow6 Direction: %s"), *Arrow6->GetComponentRotation().Vector().ToString());

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
	else
	{
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
	if (!bIsAnimating && GetActorLocation().Z <= 1761 && Velocity.SizeSquared() <= 1.0f && DiceResult == EDiceResult::None)
	{
		DiceResult = GetDiceResult();
		UE_LOG(LogTemp, Warning, TEXT("Result : %hhd"), DiceResult);

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

		const FVector NewPosition = FMath::Lerp(StartPosition, TargetPosition, Alpha);
		const FRotator NewRotation = FMath::Lerp(StartRotation, TargetRotation, Alpha);
		SetActorLocationAndRotation(NewPosition, NewRotation);
		
		if (Alpha >= 1.0f)
		{
			bIsAnimating = false;
			ElapsedTime = 0.0f;
			DropDice();
		}
	}
}
