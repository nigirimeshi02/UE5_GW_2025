// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Walking/EnemyWalkingShooter.h"
#include "EnemyWalkingShooterSniper.generated.h"

class AMyLineDrawer;

/**
 * 敵スナイパー。射程が長く、発射間隔が長い。
 * Shippingビルドでも見えるレーザーサイト（メッシュ変形）を実装。
 */
UCLASS()
class UE5_GW_2025_API AEnemyWalkingShooterSniper : public AEnemyWalkingShooter
{
	GENERATED_BODY()

public:
	// コンストラクタ
	AEnemyWalkingShooterSniper();

protected:
	// BeginPlay() でアセットの適用とパラメータ設定を行う
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void TryShootAtPlayer() override;


private:
	// --- スナイパー固有の特性 ---

	UPROPERTY(EditDefaultsOnly, Category = "Sniper Combat")
	float SniperFireRange = 6000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Sniper Combat")
	float SniperFireInterval = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Sniper Combat")
	int32 SniperBurstCount = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Sniper Combat")
	float SniperFireCycleInterval = 5.0f;

	// 発射後の非表示時間を管理するタイマーハンドル
	FTimerHandle TH_LaserCooldown;

	// レーザーを表示しても良いかどうかのフラグ
	UPROPERTY(VisibleAnywhere, Category = "Sniper Combat")
	bool bCanShowLaser = true;

	// 発射後のレーザー非表示時間
	UPROPERTY(EditDefaultsOnly, Category = "Sniper Combat")
	float LaserHideDuration = 1.0f;

	// レーザー再表示を許可する関数
	void AllowLaserDisplay();

	// --- 【修正】レーザー表示用アセット指定変数 ---

	// ★重要: Shipping対策。C++でパスを書かず、BPでこの変数に「Cylinder」を指定する
	UPROPERTY(EditDefaultsOnly, Category = "Sniper Setup")
	TObjectPtr<UStaticMesh> LaserBaseMesh;

	// ★重要: Shipping対策。BPでこの変数に「M_SniperLaser」を指定する
	UPROPERTY(EditDefaultsOnly, Category = "Sniper Setup")
	TObjectPtr<UMaterialInterface> LaserBaseMaterial;


	// --- レーザー表示用コンポーネント ---

	// レーザーの実体となるコンポーネント
	UPROPERTY(VisibleAnywhere, Category = "Sniper Combat|Visuals")
	TObjectPtr<UStaticMeshComponent> LaserMeshComponent;

	// レーザーの色を動的に変えるためのマテリアルインスタンス
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> LaserMaterialInstance;

public:
	// エイム予測レーザービームを表示（更新）する関数
	void ShowAimPredictor(float DeltaTime);

	// デバッグ用ラインドロワー（必要に応じて残す）
	UPROPERTY(Transient)
	TObjectPtr<AMyLineDrawer> LineDrawer;

};