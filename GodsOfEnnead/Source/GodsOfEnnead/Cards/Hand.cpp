#include "Hand.h"

void UHand::AddCard(ACardActor* Card, const FVector& Position)
{
    CardPositions.Add(FCardPosition(Position, Card));
	CardsInHand++;
}

void UHand::RemoveCard(ACardActor* Card)
{
	CardPositions.RemoveAll([Card](const FCardPosition& CardPosition)
	{
		return CardPosition.CardActor == Card;
	});
	CardsInHand--;
}
