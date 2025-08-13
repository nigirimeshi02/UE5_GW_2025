// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Flying/EnemyFlying.h"
#include "EnemyFlyingShooter.generated.h"

/**
 * 
 */
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<AActor> BulletClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireRange = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireInterval = 2.0f;

private:
    FTimerHandle FireTimerHandle;
};
