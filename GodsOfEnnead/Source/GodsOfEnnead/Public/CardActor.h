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
	void MoveToHand(FVector TargetLocation);
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "InteractiveObjectsActor")
	UStaticMeshComponent* Object;
	UPROPERTY(VisibleAnywhere, Category = "InteractiveObjectsActor")
	UBoxComponent* CollisionComponent;

	void AnimateTo(const FVector& StartPos, const FVector& FinalPos);
	virtual void Tick(float DeltaTime) override;

private:
	bool bIsAnimating = false;
	FVector StartPosition;
	FVector TargetPosition;
	float ElapsedTime;
	const float AnimationDuration = 1.0f;

};