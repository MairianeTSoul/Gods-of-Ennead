#include "Hand.h"

#include "Task.h"
#include "TaskUserWidget.h"

void UHand::AddCard(ACardActor* Card, const FVector& Position)
{
    CardPositions.Add(FCardPosition(Position, Card));
	CardsInHand++;
}

void UHand::RemoveCard(ACardActor* Card)
{
	FCardPosition* FoundPosition = CardPositions.FindByPredicate([Card](const FCardPosition& CardPosition)
	{
		return CardPosition.CardActor == Card;
	});

	if (FoundPosition)
	{
		FoundPosition->CardActor = nullptr;
		CardsInHand--;
		if (CardsInHand >= g_maxInHand)
		{
			for (int32 i = 0; i < CardsInHand; ++i)
			{
				if (CardPositions[i].CardActor == nullptr)
				{
					CardPositions[i].CardActor = CardPositions[i + 1].CardActor;
					if (CardPositions[i].CardActor)
					{
						CardPositions[i].CardActor->SetActorLocation(CardPositions[i].Position);
					}
					CardPositions[i + 1].CardActor = nullptr;
				}
			}

			CardPositions.RemoveAt(CardPositions.Num() - 1);
			UE_LOG(LogTemp, Log, TEXT("Пустая позиция заполнена, последняя ячейка удалена."));
		}
	}
	
	
}

void UHand::MoveToHand(ACardActor* CardActor)
{
	for (FCardPosition& Position : CardPositions)
	{
		if (Position.CardActor == nullptr)
		{
			Position.CardActor = CardActor;
			CardActor->SetActorLocation(Position.Position);
			UE_LOG(LogTemp, Log, TEXT("Карта перемещена на свободное место."));
			return;
		}
	}

	FVector LastCardPos = CardPositions.Last().Position;
	FVector NewPosition = LastCardPos + FVector(0.0f, 500.0f, 0.0f);
	AddCard(CardActor, NewPosition);
	CardActor->SetActorLocation(NewPosition);
	UE_LOG(LogTemp, Log, TEXT("Карта добавлена в конец руки. %d"), CardsInHand);
}

void UHand::MoveToDeck(ACardActor* CardActor, const FVector& NewLocation)
{
	RemoveCard(CardActor);
	CardActor->SetActorLocation(NewLocation + FVector(0.0f, 0.0f, 1.0f));
}

bool UHand::CheckTask(UTask* Task)
{
	TArray<FDataCardStruct> Cards;
	for (const FCardPosition& Position : CardPositions)
	{
		if (Position.CardActor)
		{
			Cards.Add(Position.CardActor->GetDataCard());
		}
	}

	return Task->CheckTaskCompletion(Cards);
}
