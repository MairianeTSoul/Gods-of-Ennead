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


void ACardActor::MoveToHand(int32 cardNum)
{
	FVector TargetLocation;
	if (GetActorLocation().X < 7500.0f)
	{
		TargetLocation = FVector(7569.0f, 7538.0f, 563.0f + gNum*0.3f);
		gNum++;
	}
	else
	{
		TargetLocation = FVector(7280.0f, 5676.0f + cardNum * 500.0f, 563.0f + cardNum * 0.1f);
	}
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