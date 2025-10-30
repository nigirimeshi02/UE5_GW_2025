// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Flying/EnemyFlying.h"
#include "EnemyFlyingBomb.generated.h"

/**
 * 
 */
UCLASS()
class UE5_GW_2025_API AEnemyFlyingBomb : public AEnemyFlying
{
	GENERATED_BODY()

public:
	AEnemyFlyingBomb();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void Die() override;

	/** 爆発するプレイヤーとの距離（半径） */
	UPROPERTY(EditAnywhere, Category = "Bomb")
	float ExplosionRadius = 250.0f; // 爆発トリガーとなる距離

	/** 一度だけ爆発したかどうか */
	UPROPERTY(VisibleAnywhere, Category = "Bomb") // VisibleAnywhereに変更
		bool bHasExploded = false; // フラグ名を変更し、エディタで確認可能に

	// --- カウントダウン用プロパティ ---

	/** 爆発までのカウントダウン時間 */
	UPROPERTY(EditAnywhere, Category = "Bomb")
	float ExplosionFuseTime = 2.0f; // 爆発までの時間（例: 2.0秒）

	/** 現在のカウントダウンの残り時間 */
	float CurrentFuseTime = 0.0f;

	/** カウントダウンが開始されたかどうか */
	bool bIsCountingDown = false;

	/** 爆発処理を実行し、自身に高ダメージを与える */
	void ExplodeAndDestroy();

	/** カウントダウン開始時の初期処理 */
	void StartFuse();
};
