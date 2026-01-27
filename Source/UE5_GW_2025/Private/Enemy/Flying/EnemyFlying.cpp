#include "Enemy/Flying/EnemyFlying.h"
#include "Enemy/EnemyStateMachineComponent.h" 

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

    if (!StateMachine) return;
    if (CurrentHealth <= 0) return;

    AActor* Target = StateMachine->GetTarget();

    if (Target)
    {
        // ターゲットがいるなら移動ロジックにお任せ
        MoveToTarget(Target, DeltaTime);
    }
    else
    {
        // --- ターゲットがいない時の待機ホバリング処理 ---

        // 時間経過を取得
        float Time = GetWorld()->GetTimeSeconds();

        // 1. 「今の瞬間のサイン波の高さ」と「次の瞬間のサイン波の高さ」の差分を計算します。
        // これにより、現在位置を基準にして上下に動かすことができます。

        float CurrentSine = FMath::Sin(Time * HoverOscillationSpeed) * HoverAmplitude;
        float NextSine = FMath::Sin((Time + DeltaTime) * HoverOscillationSpeed) * HoverAmplitude;

        // 差分（どれだけ動くか）
        float ZOffset = NextSine - CurrentSine;

        // ワールド座標でZ軸だけずらす
        AddActorWorldOffset(FVector(0.f, 0.f, ZOffset), true);
    }
}

void AEnemyFlying::MoveToTarget(AActor* Target, float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    FVector TargetLocation = Target->GetActorLocation();
    FVector NewLocation = CurrentLocation;

    // 1. 高さ（Z軸）の制御
    // ターゲットの頭上(HoverHeight)を基準位置にする
    float TargetZ = TargetLocation.Z + HoverHeight;
    float SmoothedZ = FMath::FInterpTo(CurrentLocation.Z, TargetZ, DeltaTime, 2.0f);

    // ここでは絶対位置を指定するので、Sine波をそのまま足す
    float Time = GetWorld()->GetTimeSeconds();
    float HoverOffset = FMath::Sin(Time * HoverOscillationSpeed) * HoverAmplitude;

    NewLocation.Z = SmoothedZ + HoverOffset;

    // 2. 水平移動
    FVector HorizontalToTarget = FVector(TargetLocation.X, TargetLocation.Y, 0) - FVector(CurrentLocation.X, CurrentLocation.Y, 0);
    float Distance2D = HorizontalToTarget.Size();

    if (Distance2D > StopDistance)
    {
        FVector HorizontalDirection = HorizontalToTarget.GetSafeNormal();
        NewLocation.X += HorizontalDirection.X * FlySpeed * DeltaTime;
        NewLocation.Y += HorizontalDirection.Y * FlySpeed * DeltaTime;
    }

    // 3. 回転
    if (!HorizontalToTarget.IsNearlyZero())
    {
        FRotator TargetRotation = HorizontalToTarget.Rotation();
        FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.0f);
        SetActorRotation(NewRotation);
    }

    SetActorLocation(NewLocation, true);
}

void AEnemyFlying::OnPlayerLost()
{
    if (StateMachine)
    {
        StateMachine->ChangeState(EEnemyState::Idle);
        StateMachine->SetTarget(nullptr);
        StateMachine->ChangeState(EEnemyState::Search);
    }
}