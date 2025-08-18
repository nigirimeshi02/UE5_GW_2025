#include "Enemy/Walking/EnemyWalkingShooter.h"
#include "Enemy/EnemyStateMachineComponent.h"
#include "Enemy/EnemyWeaponComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AEnemyWalkingShooter::AEnemyWalkingShooter()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyWalkingShooter::BeginPlay()
{
    Super::BeginPlay();

    // 一定間隔で射撃を試みる
    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyWalkingShooter::TryShootAtPlayer, FireInterval, true);
}

void AEnemyWalkingShooter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!StateMachine) return;

    AActor* Target = StateMachine->GetTarget();
    if (!Target) return;

    // 回転テスト
    // 水平方向のベクトル
    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
    ToTarget.Z = 0;

    if (!ToTarget.IsNearlyZero())
    {
        // ターゲット方向の回転
        FRotator TargetRotation = ToTarget.Rotation();

        // 現在の回転を滑らかに補間
        FRotator NewRotation = FMath::RInterpTo(
            GetActorRotation(),
            TargetRotation,
            DeltaTime,
            5.0f // ← 回転速度（大きいほど速く向く）
        );

        // テスト回転
		MyRotation = NewRotation;

        // コントローラーの向きを更新（アニメBP側で使用）
        if (Controller)
        {
            Controller->SetControlRotation(NewRotation);
			SetActorRotation(NewRotation);
            //UE_LOG(LogTemp, Log, TEXT("%f %f %f"),NewRotation.Roll,NewRotation.Yaw,NewRotation.Pitch);
        }
    }


    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

    // 攻撃範囲に入ったら Attack 状態に
    if (Distance <= FireRange)
    {
        if (StateMachine->GetCurrentState() != EEnemyState::Attack)
        {
            StateMachine->ChangeState(EEnemyState::Attack);
        }
    }
    else
    {
        // 範囲外なら追跡に戻す
        if (StateMachine->GetCurrentState() == EEnemyState::Attack)
        {
            StateMachine->ChangeState(EEnemyState::Chase);
        }
    }
}

void AEnemyWalkingShooter::TryShootAtPlayer()
{
    if (!StateMachine) return;

    if (StateMachine->GetCurrentState() != EEnemyState::Attack)
    {
        return; // 攻撃状態じゃないと撃たない
    }

    AActor* Target = StateMachine->GetTarget();
    if (!Target) return;

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance > FireRange) return;

    // 銃の方向を向く（視線）
    FRotator LookAt = (Target->GetActorLocation() - GetActorLocation()).Rotation();
    //SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));

    // 弾を発射
    if (BulletClass)
    {
        FVector MuzzleLocation = GetActorLocation() + GetActorForwardVector() * 200.f + FVector(0, 0, 50.f);
        FActorSpawnParameters SpawnParams;
        GetWorld()->SpawnActor<AActor>(BulletClass, MuzzleLocation, LookAt, SpawnParams);

        UE_LOG(LogTemp, Log, TEXT("EnemyShooter: Fired!"));
    }
}
