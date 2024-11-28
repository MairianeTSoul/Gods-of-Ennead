#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CardActor.generated.h"

class UBoxComponent;
inline int32 gNum = 1;
UCLASS(Blueprintable)
class GODSOFENNEAD_API ACardActor : public AActor
{
	GENERATED_BODY()
	
public:
	ACardActor();
	void MoveToHand(int32 cardNum);
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "InteractiveObjectsActor")
	UStaticMeshComponent* Object;
	UPROPERTY(VisibleAnywhere, Category = "InteractiveObjectsActor")
	UBoxComponent* CollisionComponent;
};

struct FCardPlace
{
	FVector Position;
	bool bIsPlayer = false;
	ACardActor* CardActor;
};