#include "CardActor.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"

ACardActor::ACardActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Object = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Object"));

	RootComponent = Object;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetRelativeScale3D(FVector(0.152500f, 5.092500f, 7.177500f));

	CollisionComponent->SetupAttachment(RootComponent);

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionComponent->SetGenerateOverlapEvents(true);

	Object->SetMobility(EComponentMobility::Movable);
	CollisionComponent->SetMobility(EComponentMobility::Movable);

	SetActorEnableCollision(true);
}


void ACardActor::MoveToHand(FVector TargetLocation)
{
	SetActorLocation(TargetLocation);
	UE_LOG(LogTemp, Log, TEXT("Card moved to hand: %s"), *TargetLocation.ToString());
}



void ACardActor::BeginPlay()
{
	Super::BeginPlay();

	// Включаем визуализацию коллизии
	// if (CollisionComponent)
	// {
	// 	DrawDebugBox(
	// 		GetWorld(),
	// 		CollisionComponent->GetComponentLocation(),
	// 		CollisionComponent->GetScaledBoxExtent(),
	// 		CollisionComponent->GetComponentRotation().Quaternion(),
	// 		FColor::Red, // Цвет отладочного бокса
	// 		true,        // Постоянная отрисовка
	// 		-1.0f,       // Время жизни (если не постоянно)
	// 		0,           // Приоритет отрисовки
	// 		5.0f         // Толщина линий
	// 	);
	// }
}

void ACardActor::AnimateTo(const FVector* FinalPos, const FRotator* FinalRot)
{
	bIsAnimating = true;

	StartPosition = GetActorLocation();
	StartRotation = GetActorRotation();
	
	if (FinalPos)
	{
		TargetPosition = *FinalPos;
		bAnimatePosition = true;
	}
	else
	{
		TargetPosition = StartPosition;
		bAnimatePosition = false;
	}

	if (FinalRot && FinalRot->Pitch != StartRotation.Pitch)
	{
		SetActorRotation(*FinalRot);
	}

	ElapsedTime = 0.0f;
}


void ACardActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsAnimating)
	{
		ElapsedTime += DeltaTime;
		const float Alpha = FMath::Clamp(ElapsedTime / AnimationDuration, 0.0f, 1.0f);

		if (bAnimatePosition)
		{
			const FVector NewPosition = FMath::Lerp(StartPosition, TargetPosition, Alpha);
			SetActorLocation(NewPosition);
		}

		if (bAnimateRotation)
		{
			const FRotator NewRotation = FMath::Lerp(StartRotation, TargetRotation, Alpha);
			SetActorRotation(NewRotation);
		}

		if (Alpha >= 1.0f)
		{
			bIsAnimating = false;
			ElapsedTime = 0.0f;
		}
	}
}


FDataCardStruct ACardActor::GetDataCard()
{
	return CardsData;
}

void ACardActor::SetDataCard(int hp, int attack, FString cardName)
{
	CardsData.hp = hp;
	CardsData.attack = attack;
	CardsData.cardName = cardName;
	CardsData.AssociatedActor = this;
}
