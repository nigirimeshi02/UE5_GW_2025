#pragma once

#include "CoreMinimal.h"
#include "Enemy/Walking/EnemyWalking.h"
#include "EnemyWalkingShooter.generated.h"

UCLASS()
class UE5_GW_2025_API AEnemyWalkingShooter : public AEnemyWalking
{
    GENERATED_BODY()

public:
    AEnemyWalkingShooter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // UŒ‚ˆ—
    void TryShootAtPlayer();

protected:
    // e‚Ì”­ËŠÔŠui•bj
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireInterval = 2.0f;

    // Ë’ö‹——£
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireRange = 1000.0f;

    // ’eŠÛ‚ÌƒNƒ‰ƒX
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<AActor> BulletClass;

private:
    FTimerHandle FireTimerHandle;
};
