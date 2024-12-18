#pragma once

#include "CoreMinimal.h"
#include "CardPosition.h"
#include "TaskUserWidget.h"
#include "Hand.generated.h"

inline int32 GMaxInHand = 6;
UCLASS(Blueprintable)
class GODSOFENNEAD_API UHand : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hand")
	TArray<FCardPosition> CardPositions;
	TArray<ACardActor*> FirstRow;
	TArray<ACardActor*> SecondRow;
	int CardsInHand = 0;
	int AliveCards = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hand")
	bool bIsPlayer;

	UHand() : bIsPlayer(false) {}

	void AddCard(ACardActor* Card, const FVector& Position);
	void RemoveCard(ACardActor* Card);
	void MoveToHand(ACardActor* CardActor);
	void MoveToDeck(ACardActor* CardActor, FVector& NewLocation);

	UFUNCTION(BlueprintCallable, Category = "Hand")
	bool CheckTask(UTask* Task);
	void RearrangeCards(int CardsInRow);
};
