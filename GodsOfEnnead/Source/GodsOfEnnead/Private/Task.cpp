// Fill out your copyright notice in the Description page of Project Settings.


#include "Task.h"

#include "CardActor.h"

UTask::UTask()
	: Pattern(ETaskPattern::HP), Count(3), Type(ETaskType::RUN)
{
}

void UTask::InitializeTask(ETaskPattern InPattern, int32 InCount, ETaskType InType)
{
	Pattern = InPattern;
	Count = InCount;
	Type = InType;
}

bool UTask::CheckTaskCompletion(const TArray<FDataCardStruct>& Cards, bool bIsCompleteStatusSet)
{
	TMap<FString, int32> ValueCounts;

	for (const FDataCardStruct& Card : Cards)
	{
		FString Key;
		switch (Pattern)
		{
		case ETaskPattern::HP:
			Key = FString::FromInt(Card.hp);
			break;
		case ETaskPattern::ATTACK:
			Key = FString::FromInt(Card.attack);
			break;
		case ETaskPattern::NAME:
			Key = Card.cardName;
			break;
		}

		ValueCounts.FindOrAdd(Key)++;
	}

	if (Type == ETaskType::SET)
	{
		for (const auto& Pair : ValueCounts)
		{
			if (Pair.Value >= Count)
			{
				if (bIsCompleteStatusSet) 
					bIsComplete = true;
				else 
					bIsComputerComplete = true;
				return true;
			}
		}
	}
	else if (Type == ETaskType::RUN)
	{
		TArray<int32> Keys;
		for (const auto& Key : ValueCounts)
		{
			int32 NumericKey = FCString::Atoi(*Key.Key);
			if (NumericKey != 0 || Key.Key == "0")
			{
				Keys.Add(NumericKey);
			}
		}
		Keys.Sort();

		int32 ConsecutiveCount = 1;

		for (int32 i = 1; i < Keys.Num(); ++i)
		{
			if (Keys[i] == Keys[i - 1] + 1)
			{
				ConsecutiveCount++;
				if (ConsecutiveCount >= Count)
				{
					if (bIsCompleteStatusSet) 
						bIsComplete = true;
					else 
						bIsComputerComplete = true;
					return true;
				}
			}
			else
			{
				ConsecutiveCount = 1;
			}
		}
	}

	if (bIsCompleteStatusSet) bIsComplete = false;
	else bIsComputerComplete = false;
	return false;
}

int32 UTask::GetClosestCountToCompletion(const TArray<FDataCardStruct>& Cards)
{
	TMap<FString, int32> ValueCounts;

	for (const FDataCardStruct& Card : Cards)
	{
		FString Key;
		switch (Pattern)
		{
		case ETaskPattern::HP:
			Key = FString::FromInt(Card.hp);
			break;
		case ETaskPattern::ATTACK:
			Key = FString::FromInt(Card.attack);
			break;
		case ETaskPattern::NAME:
			Key = Card.cardName;
			break;
		}

		ValueCounts.FindOrAdd(Key)++;
	}

	if (Type == ETaskType::SET)
	{
		int32 MaxCount = 0;
		for (const auto& Pair : ValueCounts)
		{
			MaxCount = FMath::Max(MaxCount, Pair.Value);
		}
		return MaxCount;
	}

	if (Type == ETaskType::RUN)
	{
		TArray<int32> Keys;
		for (const auto& Key : ValueCounts)
		{
			int32 NumericKey = FCString::Atoi(*Key.Key);
			if (NumericKey != 0 || Key.Key == "0")
			{
				Keys.Add(NumericKey);
			}
		}
		Keys.Sort();

		int32 ConsecutiveCount = 1;
		int32 MaxConsecutiveCount = 1;

		for (int32 i = 1; i < Keys.Num(); ++i)
		{
			if (Keys[i] == Keys[i - 1] + 1)
			{
				ConsecutiveCount++;
				MaxConsecutiveCount = FMath::Max(MaxConsecutiveCount, ConsecutiveCount);
			}
			else
			{
				ConsecutiveCount = 1;
			}
		}
		return MaxConsecutiveCount;
	}

	return 0;
}

bool UTask::GetStatusTask()
{
	return bIsComplete;
}

bool UTask::GetComputerStatusTask()
{
	return bIsComputerComplete;
}

FString UTask::getDescriptionTask()
{
	FString Description = "";

	if (Type == ETaskType::RUN)
	{
		Description = FString::Printf(TEXT("Task: Collect a set of %d cards with sequential %s"),
			Count, *StaticEnum<ETaskPattern>()->GetDisplayNameTextByValue(static_cast<int32>(Pattern)).ToString());
	}
	else if (Type == ETaskType::SET)
	{
		Description = FString::Printf(TEXT("Task: Collect %d cards with the same %s"),
			Count, *StaticEnum<ETaskPattern>()->GetDisplayNameTextByValue(static_cast<int32>(Pattern)).ToString());

	}
	return Description;
}