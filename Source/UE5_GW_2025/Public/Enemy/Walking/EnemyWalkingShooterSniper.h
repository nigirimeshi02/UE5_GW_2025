// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Walking/EnemyWalkingShooter.h"

#include "EnemyWalkingShooterSniper.generated.h"


class AMyLineDrawer;

/**
 * 敵スナイパー。射程が長く、発射間隔が長い。
 */
UCLASS()
class UE5_GW_2025_API AEnemyWalkingShooterSniper : public AEnemyWalkingShooter
{
	GENERATED_BODY()

public:
	// コンストラクタ
	AEnemyWalkingShooterSniper();

protected:
	// BeginPlay() で親クラスのプロパティを上書き設定
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void TryShootAtPlayer() override;



private:
	// スナイパー固有の特性（親クラスのプロパティを上書きするための値）
	// 注意: これらの値はBeginPlayで親クラスのUPROPERTYに設定されます。

	// スナイパーの射程距離
	UPROPERTY(EditDefaultsOnly, Category = "Sniper Combat")
	float SniperFireRange = 3000.0f; // 例：非常に長い射程

	// スナイパーの発射間隔（秒）
	UPROPERTY(EditDefaultsOnly, Category = "Sniper Combat")
	float SniperFireInterval = 5.0f; // 例：長い間隔

	// スナイパーのバースト発射数 (スナイパーなので基本は1発)
	UPROPERTY(EditDefaultsOnly, Category = "Sniper Combat")
	int32 SniperBurstCount = 1;

	// スナイパーのバーストごとの待機時間 (FireCycleInterval)
	UPROPERTY(EditDefaultsOnly, Category = "Sniper Combat")
	float SniperFireCycleInterval = 5.0f; // FireIntervalと合わせるか、若干短くして待機時間に使う

public:

	// **【追加】** エイム予測レーザービームを表示する関数
	void ShowAimPredictor(float DeltaTime);

	UPROPERTY(Transient) // ゲーム実行中に動的に作成/破棄されるアクターなので Transient が適切
	TObjectPtr<AMyLineDrawer> LineDrawer;

};
