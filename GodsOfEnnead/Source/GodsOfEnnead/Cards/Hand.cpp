#include "Hand.h"

#include "Task.h"
#include "TaskUserWidget.h"
#include "GodsOfEnnead/GodsOfEnneadPlayerController.h"

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
		if (CardsInHand >= GMaxInHand)
		{
			for (int32 i = 0; i < CardsInHand; ++i)
			{
				if (CardPositions[i].CardActor == nullptr)
				{
					CardPositions[i].CardActor = CardPositions[i + 1].CardActor;
					if (CardPositions[i].CardActor)
					{
						CardPositions[i].CardActor->AnimateTo(&CardPositions[i].Position);
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
			if (bIsPlayer) CardActor->AnimateTo(&Position.Position, &GShow_Rotation);
			else CardActor->AnimateTo(&Position.Position, &GHide_Rotation);
			UE_LOG(LogTemp, Log, TEXT("Карта перемещена на свободное место."));
			return;
		}
	}

	FVector LastCardPos = CardPositions.Last().Position;
	FVector NewPosition = LastCardPos + FVector(0.0f, 550.0f, 0.0f);
	AddCard(CardActor, NewPosition);
	if (bIsPlayer) CardActor->AnimateTo(&NewPosition, &GShow_Rotation);
	else CardActor->AnimateTo(&NewPosition, &GHide_Rotation);
	UE_LOG(LogTemp, Log, TEXT("Карта добавлена в конец руки. %d"), CardsInHand);
}

void UHand::MoveToDeck(ACardActor* CardActor, FVector& NewLocation)
{
	RemoveCard(CardActor);
	NewLocation += FVector(0.0f, 0.0f, 1.0f);
	CardActor->AnimateTo(&NewLocation, &GShow_Rotation);
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

	return Task->CheckTaskCompletion(Cards, bIsPlayer);
}

void UHand::RearrangeCards(int CardsInRow)
{
	constexpr float CardSpacing = 550;
	const float YShift = static_cast<float>(GMaxInHand - CardsInRow + 1) / 2 * CardSpacing;
	UE_LOG(LogTemp, Log, TEXT("shift %f"), YShift);
	UE_LOG(LogTemp, Log, TEXT("карт в ряду %d"), CardsInRow);
	UE_LOG(LogTemp, Log, TEXT("start player pos %f"), GPlayer_Card_Location.Y);
	
	for (int32 i = 0; i < CardPositions.Num(); ++i)
	{
		if (CardPositions[i].CardActor == nullptr)
		{
			for (int32 j = i + 1; j < CardPositions.Num(); ++j)
			{
				if (CardPositions[j].CardActor != nullptr)
				{
					CardPositions[i].CardActor = CardPositions[j].CardActor;
					FVector NewPos = CardPositions[i].Position;
					if (FirstRow.Num() >= CardsInRow)
					{
						if (bIsPlayer) NewPos.X -= 300;
						else NewPos.X += 300;
						NewPos.Z -= 5;
						UE_LOG(LogTemp, Log, TEXT("y index copy%d"), SecondRow.Num());
						NewPos.Y = FirstRow[SecondRow.Num()]->TargetPosition.Y;
						SecondRow.Add(CardPositions[i].CardActor);
					}
					else
					{
						FirstRow.Add(CardPositions[i].CardActor);
						NewPos.Y += YShift;
					}
					CardPositions[i].Position = NewPos;
					CardPositions[i].CardActor->AnimateTo(&CardPositions[i].Position);
					UE_LOG(LogTemp, Log, TEXT("New card Pos %f"), CardPositions[i].Position.Y);
					CardPositions[j].CardActor = nullptr;
					break;
				}
			}
		}
		else
		{
			FVector NewPos = CardPositions[i].Position;
			if (FirstRow.Num() >= CardsInRow)
			{
				if (bIsPlayer) NewPos.X -= 300;
				else NewPos.X += 300;
				NewPos.Z -= 5;
				NewPos.Y = FirstRow[SecondRow.Num()]->TargetPosition.Y;
				SecondRow.Add(CardPositions[i].CardActor);
			}
			else
			{
				FirstRow.Add(CardPositions[i].CardActor);
				NewPos.Y += YShift;
			}
			CardPositions[i].CardActor->AnimateTo(&NewPos);
			CardPositions[i].Position = NewPos;
		}
	}
}
