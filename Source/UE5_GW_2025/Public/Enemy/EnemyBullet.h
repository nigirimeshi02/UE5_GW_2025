// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBullet.generated.h"

UCLASS()
class UE5_GW_2025_API AEnemyBullet : public AActor
{
    GENERATED_BODY()

public:
    AEnemyBullet();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere)
    float Speed = 2000.f;

    UPROPERTY(EditAnywhere)
    float LifeTime = 3.0f;

private:
    FVector Velocity;
};
