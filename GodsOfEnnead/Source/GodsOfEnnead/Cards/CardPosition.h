#pragma once

#include "CoreMinimal.h"
#include "CardActor.h"
#include "CardPosition.generated.h"

USTRUCT(BlueprintType)
struct FCardPosition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Card Position")
	FVector Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Card Position")
	ACardActor* CardActor;

	FCardPosition(const FVector& InPosition = FVector::ZeroVector, ACardActor* InCardActor = nullptr)
		: Position(InPosition), CardActor(InCardActor) {}
};
