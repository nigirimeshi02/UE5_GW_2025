// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBullet.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class UE5_GW_2025_API AEnemyBullet : public AActor
{
    GENERATED_BODY()

public:
    AEnemyBullet();

protected:
    virtual void BeginPlay() override;

    // ヒットイベント
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);

public:
    virtual void Tick(float DeltaTime) override;

    // 当たり判定
    UPROPERTY(VisibleDefaultsOnly, Category = "Collision")
    USphereComponent* CollisionComp;

    // 移動処理
    UPROPERTY(VisibleAnywhere, Category = "Movement")
    UProjectileMovementComponent* ProjectileMovement;

    // 弾の速度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
    float Speed = 1000.0f;

    // 弾の寿命
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
    float LifeTime = 3.0f;

    // ダメージ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
    float Damage = 10.0f;

private:
    FVector Velocity;
};
