#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ETurnStatus : uint8
{
	Waiting         UMETA(DisplayName = "Waiting"),
	Waiting_Choose  UMETA(DisplayName = "Waiting Choose"),
	Player_Turn     UMETA(DisplayName = "Player Turn"),
	Computer_Turn   UMETA(DisplayName = "Computer Turn")
};

