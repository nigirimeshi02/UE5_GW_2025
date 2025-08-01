// EnemyState.h
#pragma once

#include "EnemyState.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    Idle     UMETA(DisplayName = "Idle"),
    Patrol   UMETA(DisplayName = "Patrol"),
    Chase    UMETA(DisplayName = "Chase"),
    Attack   UMETA(DisplayName = "Attack"),
    Search   UMETA(DisplayName = "Search"),
    Dead     UMETA(DisplayName = "Dead")
};

