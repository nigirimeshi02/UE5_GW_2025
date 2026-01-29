#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "EnemyFlying.generated.h"

/**
 * 飛行する敵の基底クラス
 * ターゲット（プレイヤー）に対して空中で接近し、一定距離でホバリングする基本動作を持つ
 */
UCLASS()
class UE5_GW_2025_API AEnemyFlying : public AEnemyBase
{
    GENERATED_BODY()

public:
    AEnemyFlying();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

	virtual void Die() override;

    // プレイヤーをロストしたとき呼ばれる
    virtual void OnPlayerLost() override;

    // --- 新規追加: 移動ロジック関数（継承クラスでオーバーライド可能にする） ---
    // Bombクラスではここを書き換えて「突撃」を実装します
    virtual void MoveToTarget(AActor* Target, float DeltaTime);


    // --- パラメータ群 ---

    // ホバリングの揺れの強さと速さ
    UPROPERTY(EditAnywhere, Category = "Flying")
    float HoverAmplitude = 6.0f; // 揺れ幅（上下の振れ幅）

    UPROPERTY(EditAnywhere, Category = "Flying")
    float HoverOscillationSpeed = 2.0f; // 揺れの速さ（周波数）

    UPROPERTY(EditAnywhere, Category = "Flying")
    float HoverHeight = 200.0f; // プレイヤーからどの高さにホバリングするか

    UPROPERTY(EditAnywhere, Category = "Flying")
    float FlySpeed = 300.0f; // 水平移動の速度

    // プレイヤーの手前で停止する距離
    UPROPERTY(EditAnywhere, Category = "Flying")
    float StopDistance = 600.0f;

    // ホバリング音を再生するためのコンポーネント
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
    UAudioComponent* HoverAudioComponent;

    // エディタで設定するサウンドアセット
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* HoverSoundCue;

    // 基本の音量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float BaseHoverVolume = 1.0f;
};