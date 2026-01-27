#include "Enemy/Flying/EnemyFlyingShooter.h"

// 必要なヘッダー
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h" 
#include "Enemy/EnemyStateMachineComponent.h" 

AEnemyFlyingShooter::AEnemyFlyingShooter()
{
    PrimaryActorTick.bCanEverTick = true;

    GunMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMeshComponent"));

    if (RootComponent)
    {
        GunMeshComponent->SetupAttachment(RootComponent);
    }
    else
    {
        SetRootComponent(GunMeshComponent);
    }

    // デフォルト補正値（エディタで調整可能）
    MeshRotationOffset = FRotator(0.f, -90.f, 0.f);
}

void AEnemyFlyingShooter::BeginPlay()
{
    Super::BeginPlay();
    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyFlyingShooter::TryShootAtPlayer, FireInterval, true);

    // --- 追加: ゲーム開始時に、即座に正面（進行方向）を向かせる ---
    // これをやらないと、動き出すまで一瞬横を向いてしまうことがあります
    FRotator InitialRotation = GetActorRotation() + MeshRotationOffset;
    GunMeshComponent->SetWorldRotation(InitialRotation);
}

void AEnemyFlyingShooter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!StateMachine) return;

    // --- ★追加: 死んでいたら即座にメッシュを消して処理を終わる ---
    if (StateMachine->GetCurrentState() == EEnemyState::Dead)
    {
        // まだ表示されている場合のみ実行（無駄な処理を減らすため）
        if (GunMeshComponent->IsVisible())
        {
            // 1. 見た目を消す
            GunMeshComponent->SetVisibility(false);

            // 2. 当たり判定もなくす（弾が吸われないように）
            GunMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        // 死んでいるなら、これ以降の「回転」や「攻撃判定」は不要なのでここで関数を抜ける
        return;
    }

    AActor* Target = StateMachine->GetTarget();

    // 距離判定とステート変更
    if (Target)
    {
        float Dist = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
        if (Dist <= FireRange)
        {
            if (StateMachine->GetCurrentState() != EEnemyState::Attack && StateMachine->GetCurrentState() != EEnemyState::Dead)
                StateMachine->ChangeState(EEnemyState::Attack);
        }
        else
        {
            if (StateMachine->GetCurrentState() == EEnemyState::Attack)
                StateMachine->ChangeState(EEnemyState::Chase);
        }
    }

    // --- 銃の回転制御（常に回す） ---

    FRotator BaseTargetRotation;

    // A. 攻撃中なら「プレイヤー」の方を向く
    if (StateMachine->GetCurrentState() == EEnemyState::Attack && Target)
    {
        FVector DirectionToTarget = Target->GetActorLocation() - GunMeshComponent->GetComponentLocation();
        BaseTargetRotation = DirectionToTarget.Rotation();
    }
    // B. それ以外（最初の段階や移動中）は「敵自身の進行方向」を向く
    else
    {
        BaseTargetRotation = GetActorRotation();
    }

    // ★重要: どちらの場合でも「メッシュのズレ補正」を足す
    FRotator FinalGoalRotation = BaseTargetRotation + MeshRotationOffset;

    // 現在の回転からスムーズに補間
    FRotator NewRotation = FMath::RInterpTo(
        GunMeshComponent->GetComponentRotation(),
        FinalGoalRotation,
        DeltaTime,
        RotationSpeed
    );

    GunMeshComponent->SetWorldRotation(NewRotation);
}

void AEnemyFlyingShooter::TryShootAtPlayer()
{
    if (!StateMachine || StateMachine->GetCurrentState() != EEnemyState::Attack)
        return;

    AActor* Target = StateMachine->GetTarget();
    if (!Target || !BulletClass) return;

    // --- 修正箇所: 弾の出現位置を計算 ---

    // 1. 銃（メッシュ）の位置とターゲットの位置を取得
    FVector GunLoc = GunMeshComponent->GetComponentLocation();
    FVector TargetLoc = Target->GetActorLocation();

    // 2. ターゲットへの方向ベクトル（長さ1の単位ベクトル）を計算
    FVector DirectionToTarget = (TargetLoc - GunLoc).GetSafeNormal();

    // 3. 出現位置を決定
    // 方向ベクトルに距離（例: 150.0f）を掛けた分だけ、銃の位置から離します
    // ※もし敵が大きい場合は、この数値を 200.0f や 300.0f に増やしてください
    float SpawnDistance = 150.0f;
    FVector MuzzleLoc = GunLoc + (DirectionToTarget * SpawnDistance);

    // 4. 弾の回転（ターゲットの方を向く）
    FRotator BulletRotation = DirectionToTarget.Rotation();

    // 5. スポーンパラメータの設定（コリジョン対策）
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;            // 自分をオーナーに設定
    SpawnParams.Instigator = GetInstigator();
    // 多少重なっていても強制的にスポーンさせる設定
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 弾を発射
    GetWorld()->SpawnActor<AActor>(BulletClass, MuzzleLoc, BulletRotation, SpawnParams);
}