#include "Enemy/Flying/EnemyFlyingBomb.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
// これがないとクラッシュの原因になります
#include "Enemy/EnemyStateMachineComponent.h" 

AEnemyFlyingBomb::AEnemyFlyingBomb()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyFlyingBomb::BeginPlay()
{
    Super::BeginPlay();

    // 自爆ドローン用の動き設定
    HoverAmplitude = 3.0f;
    HoverOscillationSpeed = 5.0f; // 激しく揺れる
    HoverHeight = 50.0f;          // プレイヤーの胴体高さを狙う
    FlySpeed = 500.0f;            // 少し速く
}

void AEnemyFlyingBomb::Tick(float DeltaTime)
{
    // 死んでいたら処理しない
    if (CurrentHealth <= 0) return;

    // ★親クラスのTickを呼ぶことで、MoveToTarget（移動）とステート管理を行わせる
    Super::Tick(DeltaTime);

    // --- ここからは「自爆」固有のロジックのみ記述 ---

    // カウントダウン処理
    if (bIsCountingDown && !bHasExploded)
    {
        CurrentFuseTime -= DeltaTime;

        // ここで「赤く点滅する」などの演出を入れると良い感じです

        if (CurrentFuseTime <= 0.0f)
        {
            ExplodeAndDestroy();
        }
    }
}

// ★親クラスの移動ロジックを「突撃」用に完全に書き換える
void AEnemyFlyingBomb::MoveToTarget(AActor* Target, float DeltaTime)
{
    if (!Target) return;

    FVector CurrentLocation = GetActorLocation();
    FVector TargetLocation = Target->GetActorLocation();

    // 1. 距離チェック（自爆スイッチの起動判定）
    float Dist = FVector::Dist(CurrentLocation, TargetLocation);
    if (Dist <= ExplosionRadius)
    {
        StartFuse();
    }

    // 2. 移動計算
    FVector NewLocation = CurrentLocation;

    // 高さ: 相手の高さへ直接向かう（親クラスのような上空待機はしない）
    float TargetZ = TargetLocation.Z + HoverHeight;
    // 上下移動も素早く追従させる
    NewLocation.Z = FMath::FInterpTo(CurrentLocation.Z, TargetZ, DeltaTime, 5.0f);

    // 水平移動: 「停止距離」の判定を行わず、常にターゲットへ加算し続ける
    FVector HorizontalToTarget = FVector(TargetLocation.X, TargetLocation.Y, 0) - FVector(CurrentLocation.X, CurrentLocation.Y, 0);
    FVector MoveDirection = HorizontalToTarget.GetSafeNormal();

    // 突撃移動
    NewLocation.X += MoveDirection.X * FlySpeed * DeltaTime;
    NewLocation.Y += MoveDirection.Y * FlySpeed * DeltaTime;

    // 3. 回転（常にターゲットを見る）
    if (!HorizontalToTarget.IsNearlyZero())
    {
        FRotator TargetRot = HorizontalToTarget.Rotation();
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 10.0f));
    }

    // 移動適用（Sweep=trueでめり込み防止）
    SetActorLocation(NewLocation, true);
}

void AEnemyFlyingBomb::StartFuse()
{
    if (bHasExploded || bIsCountingDown) return;

    bIsCountingDown = true;
    CurrentFuseTime = ExplosionFuseTime;

    UE_LOG(LogTemp, Warning, TEXT("Bomb Fuse Started!"));

    // カウントダウン開始音などをここで再生すると効果的です
}

void AEnemyFlyingBomb::ExplodeAndDestroy()
{
    if (bHasExploded) return;
    bHasExploded = true;

    // 1. 周囲へのダメージ
    UGameplayStatics::ApplyRadialDamage(
        GetWorld(),
        100.0f, // 爆発ダメージ
        GetActorLocation(),
        ExplosionRadius + 200.0f, // ダメージ範囲はトリガー範囲より少し広めに
        UDamageType::StaticClass(),
        TArray<AActor*>(), // 無視リスト
        this,              // ダメージ発生源
        GetInstigatorController()
    );

    UE_LOG(LogTemp, Warning, TEXT("BOOM! Enemy exploded."));

    // 2. 自身を破壊するために特大ダメージを与える
    // (Destroy()を直接呼ぶより、Die()を経由させてエフェクトなどを出すのが綺麗です)
    UGameplayStatics::ApplyDamage(
        this,
        9999.0f,
        GetInstigatorController(),
        this,
        UDamageType::StaticClass()
    );
}

void AEnemyFlyingBomb::Die()
{
    // 親クラスのDieを呼び、スコア加算や衝突判定の無効化などを行わせる
    Super::Die();

    // メッシュを消す
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);

    // 爆発エフェクト再生
    if (ExplosionNiagaraSystem)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ExplosionNiagaraSystem,
            GetActorLocation(),
            GetActorRotation(),
            FVector(1.0f),
            true
        );
    }

    // 数秒後にActor自体を完全消去
    SetLifeSpan(2.0f);
}