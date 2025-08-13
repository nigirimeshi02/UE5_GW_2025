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
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    //// 追跡速度
    //UPROPERTY(EditAnywhere, Category = "Movement")
    //float FlySpeed = 300.0f;

    ///** ホバリングする高さ（プレイヤーより上） */
    //UPROPERTY(EditAnywhere, Category = "Flight")
    //float HoverHeight = 300.f;


    // ホバリングの揺れの強さと速さ
    UPROPERTY(EditAnywhere, Category = "Flying")
    float HoverAmplitude = 6.0f; // 揺れ幅（上下の振れ幅）

    UPROPERTY(EditAnywhere, Category = "Flying")
    float HoverOscillationSpeed = 2.0f; // 揺れの速さ（周波数）

    UPROPERTY(EditAnywhere, Category = "Flying")
    float HoverHeight = 200.0f; // プレイヤーからどの高さにホバリングするか

    UPROPERTY(EditAnywhere, Category = "Flying")
    float FlySpeed = 300.0f; // 水平移動の速度

    // プレイヤーをロストしたとき呼ばれる
    virtual void OnPlayerLost() override;
};
