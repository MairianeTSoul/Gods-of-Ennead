#include "CardActor.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"

ACardActor::ACardActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Object = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Object"));
	Object->SetupAttachment(RootComponent);
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(Object);
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionComponent->SetBoxExtent(FVector(0.152500f, 5.092500f, 7.177500f));
}

void ACardActor::MoveToHand()
{
	FVector TargetLocation = FVector(7280.0f, 5676.0f, 563.0f);
	SetActorLocation(TargetLocation);
	UE_LOG(LogTemp, Log, TEXT("Card moved to hand: %s"), *TargetLocation.ToString());
}

void ACardActor::OnCardClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	UE_LOG(LogTemp, Log, TEXT("Card clicked: %s"), *GetName());
	MoveToHand();
}