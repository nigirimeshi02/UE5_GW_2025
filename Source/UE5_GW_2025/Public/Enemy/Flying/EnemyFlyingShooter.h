#pragma once

#include "CoreMinimal.h"
#include "Enemy/Flying/EnemyFlying.h"
#include "EnemyFlyingShooter.generated.h"

UCLASS()
class UE5_GW_2025_API AEnemyFlyingShooter : public AEnemyFlying
{
    GENERATED_BODY()

public:
    AEnemyFlyingShooter();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void TryShootAtPlayer();

    // --- 追加: 武器/砲台用のスタティックメッシュ ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> GunMeshComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FRotator MeshRotationOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<AActor> BulletClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireRange = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireInterval = 2.0f;

    // --- 追加: メッシュの回転速度（スムーズに向かせるため） ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float RotationSpeed = 5.0f;

private:
    FTimerHandle FireTimerHandle;
};