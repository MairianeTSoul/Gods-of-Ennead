#pragma once

#include "CoreMinimal.h"
#include "DiceActor.h"
#include "GameFramework/Actor.h"
#include "CardActor.generated.h"

class UBoxComponent;
inline int32 gNum = 1;

USTRUCT(BlueprintType)
struct FDataCardStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere)
	int hp;
	UPROPERTY(EditAnywhere)
	int attack;
	UPROPERTY(EditAnywhere)
	FString cardName;

	UPROPERTY()
	ACardActor* AssociatedActor;
	bool operator==(const FDataCardStruct& Other) const
	{
        return hp == Other.hp && attack == Other.attack && cardName == Other.cardName && AssociatedActor == Other.AssociatedActor;
	}
};

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

	UPROPERTY(VisibleAnywhere, Category = "InteractiveObjectsActor")
	FDataCardStruct CardsData;

	UFUNCTION()
	FDataCardStruct& GetDataCard();

	UFUNCTION(BlueprintCallable, Category = "InteractiveObjectsActor")
	void SetDataCard(int hp, int attack, FString cardName);
	void Attack(ACardActor* OpponentCard, EDiceResult DiceResult);

	void AnimateTo(const FVector* FinalPos = nullptr, const FRotator* FinalRot = nullptr);
	virtual void Tick(float DeltaTime) override;

	bool bIsAlive = true;
	FVector TargetPosition;
private:
	bool bIsAnimating = false;
	bool bAnimatePosition = false;
	bool bAnimateRotation = false;
	
	FVector StartPosition;
	FRotator StartRotation;
	FRotator TargetRotation;
	float ElapsedTime;
	const float AnimationDuration = 1.0f;

};