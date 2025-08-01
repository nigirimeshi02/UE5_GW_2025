#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "EnemyWalking.generated.h"

UCLASS()
class UE5_GW_2025_API AEnemyWalking : public AEnemyBase
{
    GENERATED_BODY()

public:
    AEnemyWalking();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // ï‡çsópÇÃë¨ìxê›íËÇ»Ç«
    void SetWalkingDefaults();
};
