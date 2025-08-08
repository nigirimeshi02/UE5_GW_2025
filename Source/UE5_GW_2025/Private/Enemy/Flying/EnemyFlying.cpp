// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Flying/EnemyFlying.h"

AEnemyFlying::AEnemyFlying()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyFlying::BeginPlay()
{
    Super::BeginPlay();
    
}

//void AEnemyFlying::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    /*if (!StateMachine || StateMachine->GetCurrentState() != EEnemyState::Chase)
//        return;
//
//    AActor* Target = StateMachine->GetTarget();
//    if (!Target) return;
//
//    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
//    FVector Direction = ToTarget.GetSafeNormal();
//
//    FVector NewLocation = GetActorLocation() + Direction * FlySpeed * DeltaTime;
//    SetActorLocation(NewLocation);*/
//
//    if (!StateMachine || StateMachine->GetCurrentState() != EEnemyState::Chase)
//        return;
//
//    AActor* Target = StateMachine->GetTarget();
//    if (!Target) return;
//
//    FVector CurrentLocation = GetActorLocation();
//    FVector TargetLocation = Target->GetActorLocation();
//
//	// 目標位置までの距離が許容半径以上なら移動
//	FVector TargetLength = TargetLocation - CurrentLocation;
//    if (TargetLength.Length() >= AcceptanceRadius) {
//
//        // プレイヤーのXZ方向だけを追いかける
//        FVector HorizontalToTarget = FVector(TargetLocation.X, TargetLocation.Y, CurrentLocation.Z) - CurrentLocation;
//        FVector HorizontalDirection = HorizontalToTarget.GetSafeNormal();
//
//        // Z方向の目標位置はプレイヤー + HoverHeight
//        float TargetZ = TargetLocation.Z + HoverHeight;
//        float NewZ = FMath::FInterpTo(CurrentLocation.Z, TargetZ, DeltaTime, 2.0f); // 高さだけゆっくり補間
//
//        // 新しい位置を計算
//        FVector NewLocation = CurrentLocation + HorizontalDirection * FlySpeed * DeltaTime;
//        NewLocation.Z = NewZ;
//
//        SetActorLocation(NewLocation);
//    }
//}

void AEnemyFlying::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!StateMachine || (StateMachine->GetCurrentState() != EEnemyState::Chase 
                            && StateMachine->GetCurrentState() != EEnemyState::Attack))
        return;

    AActor* Target = StateMachine->GetTarget();
    if (!Target) return;

    FVector CurrentLocation = GetActorLocation();
    FVector TargetLocation = Target->GetActorLocation();
    FVector NewLocation = CurrentLocation; // 最終的な位置をここに組み立てていく

    // --- 常にホバリングする（Z補間 + サイン波揺れ） ---
    float TargetZ = TargetLocation.Z + HoverHeight;
    float SmoothedZ = FMath::FInterpTo(CurrentLocation.Z, TargetZ, DeltaTime, 2.0f);

    float Time = GetWorld()->GetTimeSeconds();
    float HoverOffset = FMath::Sin(Time * HoverOscillationSpeed) * HoverAmplitude;

    NewLocation.Z = SmoothedZ + HoverOffset;

    // --- 一定距離以上なら水平方向の移動・回転も行う ---
    FVector ToTarget = TargetLocation - CurrentLocation;
    if (ToTarget.Length() >= AcceptanceRadius)
    {
        // 水平移動
        FVector HorizontalToTarget = FVector(TargetLocation.X, TargetLocation.Y, CurrentLocation.Z) - CurrentLocation;
        FVector HorizontalDirection = HorizontalToTarget.GetSafeNormal();

        NewLocation += HorizontalDirection * FlySpeed * DeltaTime;

        // 回転も進行方向に
        if (!HorizontalDirection.IsNearlyZero())
        {
            FRotator NewRotation = HorizontalDirection.Rotation();
            SetActorRotation(NewRotation);
        }
    }

    SetActorLocation(NewLocation);
}

