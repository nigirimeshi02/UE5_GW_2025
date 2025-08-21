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

    // 攻撃処理
    void TryShootAtPlayer();

	void StartFireCycle();

	void BurstFire();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FRotator MyRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FVector MuzzleLocation;

protected:
    // 銃の発射間隔（秒）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireInterval = 2.0f;

    // 射程距離
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireRange = 1000.0f;

    // 弾丸のクラス
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<AActor> BulletClass;

private:
    FTimerHandle FireTimerHandle;

    FTimerHandle FireCycleTimerHandle;   // 攻撃サイクル（3発撃って1秒休む）
    FTimerHandle BurstFireTimerHandle;   // バースト内の1発ごとのタイマー
    int32 ShotsFired = 0;                // バースト内で何発撃ったか
    int32 BurstCount = 3;                // 1バーストの発射数
    float BurstInterval = 0.2f;          // 連射間隔（秒）
    float FireCycleInterval = 1.0f;      // バーストごとの待機時間
};
