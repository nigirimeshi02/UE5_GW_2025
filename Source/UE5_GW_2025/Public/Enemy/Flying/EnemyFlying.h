// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "EnemyFlying.generated.h"

/**
 * 
 */
UCLASS()
class UE5_GW_2025_API AEnemyFlying : public AEnemyBase
{
    GENERATED_BODY()

public:
    AEnemyFlying();

protected:
    virtual void Tick(float DeltaTime) override;

    // í«ê’ë¨ìx
    UPROPERTY(EditAnywhere, Category = "Movement")
    float FlySpeed = 300.0f;
};
