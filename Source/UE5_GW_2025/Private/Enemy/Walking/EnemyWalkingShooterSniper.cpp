// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Walking/EnemyWalkingShooterSniper.h"
#include "Enemy/EnemyAIController.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "Enemy/MyLineDrawer.h"

// (プロジェクト構造に応じてインクルードパスを調整してください)

AEnemyWalkingShooterSniper::AEnemyWalkingShooterSniper()
{
	// コンストラクタでは、主にコンポーネントの初期化などを行う
	// プロパティのデフォルト値を設定
	// 親クラスのPrimaryActorTick.bCanEverTick = true; は継承される

	// 継承したプロパティのデフォルト値を上書きしても良いが、
	// BeginPlayで設定する方が、エディタでの変更を無視して強制的に設定しやすい
}

void AEnemyWalkingShooterSniper::BeginPlay()
{
	// 親クラスのBeginPlayを呼び出す
	Super::BeginPlay();

	// 親クラスの戦闘プロパティをスナイパーの特性に合わせて上書き
	// これにより、親クラスの射撃ロジックがスナイパー仕様になる

	// 射程距離をスナイパー用に設定
	FireRange = SniperFireRange; 

	// 射撃間隔（FireInterval, BurstCount, FireCycleInterval）をスナイパー用に設定
	// ※親クラスのロジックでは、バースト射撃（BurstFire）でタイマーを管理しているため、
	//   FireIntervalの代わりに BurstCount と FireCycleInterval を調整します。
	
	// 1バーストの発射数を1に設定（単発）
	BurstCount = SniperBurstCount; // デフォルトは3発だが、スナイパーは1発

	// 1バースト後の待機時間（次の射撃までの時間）を長く設定
	FireCycleInterval = SniperFireCycleInterval - BurstInterval; 
	// (BurstIntervalは連射間隔で、単発の場合はほぼ無視できるため、簡略化しています)

	// UE_LOG(LogTemp, Log, TEXT("Sniper initialized: Range=%f, FireCycle=%f"), FireRange, FireCycleInterval);

	// コントローラーを取得
	if (Controller)
	{
		AEnemyAIController* EnemyAI = Cast<AEnemyAIController>(Controller);
		if (EnemyAI)
		{
			EnemyAI->SetSightRadius(3000.f);
		}
	}

	if (GetWorld())
	{
		LineDrawer = GetWorld()->SpawnActor<AMyLineDrawer>(AMyLineDrawer::StaticClass());
	}
}

void AEnemyWalkingShooterSniper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Log, TEXT("Sniper initialized: Range=%f, FireCycle=%f"), FireRange, FireCycleInterval);
	// エイム予測レーザービームの表示更新
	if (StateMachine->GetCurrentState() == EEnemyState::Attack)
	{
		ShowAimPredictor(DeltaTime);
	}
}

// Tick, TryShootAtPlayer, StartFireCycle, BurstFire などは
// 親クラス AEnemyWalkingShooter の実装をそのまま使用します。
// TryShootAtPlayer のブレ（ランダム Yaw/Pitch Offset）を調整したい場合は、
// TryShootAtPlayer をオーバーライドしてブレの値を小さくする必要があります。


// 例：精度を上げるためにTryShootAtPlayerをオーバーライドする場合
void AEnemyWalkingShooterSniper::TryShootAtPlayer()
{
	if (!StateMachine || StateMachine->GetCurrentState() != EEnemyState::Attack)
	{
		return;
	}

	AActor* Target = StateMachine->GetTarget();
	if (!Target || FVector::Dist(GetActorLocation(), Target->GetActorLocation()) > FireRange) 
	{
		return;
	}

	FRotator LookAt = (Target->GetActorLocation() - GetActorLocation()).Rotation();

	// スナイパーは精度が高いのでブレを小さくする（例：±0.5度）
	float YawOffset = FMath::FRandRange(-0.5f, 0.5f); 
	float PitchOffset = FMath::FRandRange(-0.3f, 0.3f);

	FRotator SpreadRotation = LookAt + FRotator(PitchOffset, YawOffset, 0.f);

	if (BulletClass)
	{
		FActorSpawnParameters SpawnParams;
		GetWorld()->SpawnActor<AActor>(BulletClass, MuzzleLocation, SpreadRotation, SpawnParams);
		UE_LOG(LogTemp, Log, TEXT("EnemySniper: Fired!"));

		// 発射後にレーザーを非表示にし、タイマーを開始
		bCanShowLaser = false;
		GetWorldTimerManager().SetTimer(
			TH_LaserCooldown,
			this,
			&AEnemyWalkingShooterSniper::AllowLaserDisplay,
			LaserHideDuration, // 1.0f (LaserHideDuration) 秒後に呼び出す
			false // ループしない
		);
	}
}

void AEnemyWalkingShooterSniper::AllowLaserDisplay()
{
	bCanShowLaser = true;
}

void AEnemyWalkingShooterSniper::ShowAimPredictor(float DeltaTime)
{
	AActor* Target = StateMachine->GetTarget();
	if (!Target)
	{
		return;
	}

	// ----------------------------------------------------
	// A. 狙いの方向の計算 (TryShootAtPlayerと類似)
	// ----------------------------------------------------

	// スナイパーは移動しないため、常にターゲットの方向を向く

	// 回転テスト
	// 水平方向のベクトル
	FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
	FRotator LookAt;
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

		LookAt = NewRotation;
	}

	// 予測線はブレさせず、クリーンな中心線を出すことが多い
	// ここではシンプルにターゲット方向を使用します。
	FRotator AimRotation = LookAt;
	FVector Direction = AimRotation.Vector();

	// ----------------------------------------------------
	// B. ビームパーティクルの設定
	// ----------------------------------------------------

	// ビームの開始位置 (Source)
	// MuzzleLocation は Pawn 側のプロパティですが、ここでは MuzzleLocation をローカル座標に変換して使用します
	FVector StartLocation = MuzzleLocation; // ワールド座標

	// ビームの終了位置 (Target)
	FVector EndLocation = StartLocation + Direction * FireRange;

	if (bCanShowLaser) 
	{
		LineDrawer->DrawPermanentLine(StartLocation, EndLocation, FLinearColor::Red, 5.f);
	}
}

