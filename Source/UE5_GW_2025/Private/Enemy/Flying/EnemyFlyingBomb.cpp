// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Flying/EnemyFlyingBomb.h"
#include "Kismet/GameplayStatics.h"

AEnemyFlyingBomb::AEnemyFlyingBomb()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyFlyingBomb::BeginPlay()
{
	Super::BeginPlay();

	HoverAmplitude = 3.0f;
	HoverOscillationSpeed = 3.0f;
	HoverHeight = 150.0f;
	FlySpeed = 400.0f;
}

void AEnemyFlyingBomb::Tick(float DeltaTime)
{
	if(CurrentHealth <= 0)
	{
		return; // 死亡している場合は何もしない
	}

	Super::Tick(DeltaTime);

	// ターゲット（通常はプレイヤー）が存在するか確認
	AActor* Target = StateMachine->GetTarget();
	if (!Target) return;

	//if (Target)
	//{
	//	// 現在位置とターゲットの位置を取得
	//	FVector CurrentLocation = GetActorLocation();
	//	FVector TargetLocation = Target->GetActorLocation();

	//	// ターゲットとの距離を計算
	//	float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);

	//	// 距離がExplosionRadius以下かチェック
	//	if (DistanceToTarget <= ExplosionRadius && bHasExploded)
	//	{
	//		// 爆発処理を呼び出す
	//		ExplodeAndDestroy();
	//	}
	//}

	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);

	// --- 1. 距離チェックとカウントダウン開始 ---
	if (DistanceToTarget <= ExplosionRadius)
	{
		// 範囲内に入ったら、カウントダウンを開始する
		StartFuse();
	}
	// カウントダウンが始まったら、距離外に出てもリセットしないため、
	// 以下の 'else if' ブロックを削除またはコメントアウトします。
	/*
	else if (bIsCountingDown)
	{
		// 範囲外に出たら、カウントダウンをリセットする（停止させないため削除）
		// bIsCountingDown = false;
		// FlySpeed = 400.0f;
	}
	*/


	// --- 2. カウントダウン処理 ---
	if (bIsCountingDown && !bHasExploded)
	{
		CurrentFuseTime -= DeltaTime;

		if (CurrentFuseTime <= 0.0f)
		{
			// カウントがゼロになったら爆発
			ExplodeAndDestroy();
		}
	}

	//クラッシュします
}

void AEnemyFlyingBomb::Die()
{
	Super::Die();
	// メッシュをラグドール化
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp)
	{
		MeshComp->SetVisibility(false, true);
	}

	// 爆発エフェクトやサウンドをここで再生可能
}

void AEnemyFlyingBomb::ExplodeAndDestroy()
{
	// --- 周囲に放射状ダメージを与える ---
	float BaseDamage = 1.0f;
	float DamageRadius = ExplosionRadius + 200.f;

	// クラッシュ回避のための修正点: UDamageType::StaticClass() で有効なクラスを設定
	TSubclassOf<UDamageType> DamageType0 = UDamageType::StaticClass();

	// ApplyRadialDamage の呼び出し
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		BaseDamage,
		GetActorLocation(),
		DamageRadius,
		DamageType0,
		TArray<AActor*>(),
		this,
		GetInstigatorController()
	);

	UE_LOG(LogTemp, Warning, TEXT("EnemyFlyingBomb exploded at %s!"), *GetActorLocation().ToString());

	// ----------------------------------------------------
	// ステップ 2: 自身に高ダメージを与えて死亡処理をトリガー
	// ----------------------------------------------------

	// **非常に大きなダメージ値** (敵の最大HPを確実に上回る値)
	const float SuicideDamage = 1000.0f;

	// ダメージタイプ（必要に応じて専用のUDamageTypeを定義する）
	TSubclassOf<UDamageType> DamageType1 = UDamageType::StaticClass();

	// UGameplayStatics::ApplyDamage を使用して自身にダメージを適用
	UGameplayStatics::ApplyDamage(
		this,					  // ダメージを受けるActor (自身)
		SuicideDamage,			  // ダメージ量
		GetInstigatorController(),// ダメージを与えたController (Instigatorが設定されていればそれを使用)
		this,					  // ダメージを与えたActor (自身)
		DamageType1				  // ダメージタイプ
	);

	bHasExploded = true; // 一度だけ爆発するようにフラグを更新
}

void AEnemyFlyingBomb::StartFuse()
{
	// 既に爆発済みか、カウントダウン中の場合は無視
	if (bHasExploded || bIsCountingDown)
	{
		return;
	}

	bIsCountingDown = true;
	CurrentFuseTime = ExplosionFuseTime;

	// 例: カウントダウン開始時に移動を停止する
	FlySpeed = 0.0f;

	// 例: 警告音や点滅エフェクトを開始する
	// PlayWarningSound();
	// StartWarningLight(); 

	UE_LOG(LogTemp, Warning, TEXT("EnemyFlyingBomb fuse started! %f seconds to detonation."), ExplosionFuseTime);
}
