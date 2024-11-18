#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CardActor.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class GODSOFENNEAD_API ACardActor : public AActor
{
	GENERATED_BODY()
	
public:
	ACardActor();
	void MoveToHand();
	void OnCardClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	UPROPERTY(VisibleAnywhere, Category = "InteractiveObjectsActor")
	UStaticMeshComponent* Object;
	UPROPERTY(VisibleAnywhere, Category = "InteractiveObjectsActor")
	UBoxComponent* CollisionComponent;
};
