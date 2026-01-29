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

        // --- ここから音のロジック ---
        if (BeepSound)
        {
            BeepTimer += DeltaTime;

            // 残り時間に基づいて現在の間隔を計算 (Lerpを使用)
            // CurrentFuseTime が ExplosionFuseTime(初期値) に近いほど MaxInterval
            // 0 に近いほど MinInterval になるように補完します
            float Alpha = FMath::Clamp(CurrentFuseTime / ExplosionFuseTime, 0.0f, 1.0f);
            float CurrentInterval = FMath::Lerp(MinBeepInterval, MaxBeepInterval, Alpha);

            if (BeepTimer >= CurrentInterval)
            {
                // 音を再生
                // 修正前: UGameplayStatics::PlaySoundAtLocation(this, BeepSound, GetActorLocation());

                // 修正後: 引数をすべて指定し、最後に減衰設定を渡します
                UGameplayStatics::PlaySoundAtLocation(
                    this,
                    BeepSound,
                    GetActorLocation(),
                    GetActorRotation(),
                    0.5f, // 音量倍率
                    1.0f, // ピッチ倍率
                    0.0f, // 開始時間オフセット
                    BombAttenuationSettings // ★ここに減衰アセットを渡す
                );

                // タイマーリセット
                BeepTimer = 0.0f;

                // 【ヒント】ここでBP側に「今ピッと言ったよ！」という通知を送ると、
                // BPの点滅タイムラインと完全に同期させることができます。
                OnBeepUpdate();
            }
        }
        // --- ここまで ---

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

    // --- 爆発SEの再生 ---
    if (ExplosionSound)
    {
        // 修正前: UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

        // 修正後
        UGameplayStatics::PlaySoundAtLocation(
            this,
            ExplosionSound,
            GetActorLocation(),
            GetActorRotation(),
            0.5f,
            1.0f,
            0.0f,
            BombAttenuationSettings // ★ここに減衰アセットを渡す
        );
    }

    // 数秒後にActor自体を完全消去
    SetLifeSpan(2.0f);
}