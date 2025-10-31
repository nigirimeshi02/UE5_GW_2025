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

void AEnemyFlying::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //if (!StateMachine || (StateMachine->GetCurrentState() != EEnemyState::Chase 
    //                        && StateMachine->GetCurrentState() != EEnemyState::Attack))
    //    return;

    AActor* Target = StateMachine->GetTarget();
    FVector CurrentLocation = GetActorLocation();
    FVector NewLocation = CurrentLocation; // 最終的な位置をここに組み立てていく
    if (Target) {

        
        FVector TargetLocation = Target->GetActorLocation();
        

        // --- 常にホバリングする（Z補間 + サイン波揺れ） ---
        float TargetZ = TargetLocation.Z + HoverHeight;
        float SmoothedZ = FMath::FInterpTo(CurrentLocation.Z, TargetZ, DeltaTime, 2.0f);

        float Time = GetWorld()->GetTimeSeconds();
        float HoverOffset = FMath::Sin(Time * HoverOscillationSpeed) * HoverAmplitude;

        NewLocation.Z = SmoothedZ + HoverOffset;

        // --- 一定距離以上なら水平方向の移動・回転も行う ---
        FVector ToTarget = TargetLocation - CurrentLocation;
        if (/*ToTarget.Length() >= AcceptanceRadius*/true)
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
    }
    else
    {
        // ターゲットがいない場合はホバリングのみ維持
        float Time = GetWorld()->GetTimeSeconds();
        float HoverOffset = FMath::Sin(Time * HoverOscillationSpeed) * HoverAmplitude;
		NewLocation.Z = CurrentLocation.Z + HoverOffset;
    }

    SetActorLocation(NewLocation, true);
}

void AEnemyFlying::OnPlayerLost()
{
    if (StateMachine)
    {
        StateMachine->ChangeState(EEnemyState::Idle);
        StateMachine->SetTarget(nullptr);
    }

    // 高度を維持する場合（落下防止）
    FVector CurrentLocation = GetActorLocation();
    SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z));

    // 状態遷移：探し中
    StateMachine->ChangeState(EEnemyState::Search);
    
}

