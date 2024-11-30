#pragma once

#include "CoreMinimal.h"
#include "CardPosition.h"
#include "Hand.generated.h"

UCLASS(Blueprintable)
class GODSOFENNEAD_API UHand : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hand")
	TArray<FCardPosition> CardPositions;
	int CardsInHand = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hand")
	bool bIsPlayer;

	UHand() : bIsPlayer(false) {}

	void AddCard(ACardActor* Card, const FVector& Position);
	void RemoveCard(ACardActor* Card);
};
