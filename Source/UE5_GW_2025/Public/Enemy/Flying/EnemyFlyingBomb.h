#pragma once

#include "CoreMinimal.h"
#include "Enemy/Flying/EnemyFlying.h"
#include "EnemyFlyingBomb.generated.h"

/**
 * 自爆型の飛行敵
 * ターゲットに向かって一直線に突撃し、一定距離で起爆カウントダウンを開始する
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

    // ★重要: 親クラスの移動ロジックを上書きして「突撃」挙動にする
    virtual void MoveToTarget(AActor* Target, float DeltaTime) override;

    // --- パラメータ ---

    /** 爆発トリガーとなる距離 */
    UPROPERTY(EditAnywhere, Category = "Bomb")
    float ExplosionRadius = 200.0f; // 確実に当てるため少し狭め、あるいは広めに調整

    /** カウントダウン時間 */
    UPROPERTY(EditAnywhere, Category = "Bomb")
    float ExplosionFuseTime = 2.0f;

    /** カウントダウン中の現在時間 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bomb")
    float CurrentFuseTime = 0.0f;

    /** カウントダウン中か？ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bomb")
    bool bIsCountingDown = false;

    /** 爆発済みフラグ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bomb")
    bool bHasExploded = false;

    // --- 関数 ---

    /** 爆発処理（ダメージ発生＋自身死亡） */
    void ExplodeAndDestroy();

    /** カウントダウン開始 */
    void StartFuse();

    /** 爆発エフェクト */
    UPROPERTY(EditDefaultsOnly, Category = "FX")
    class UNiagaraSystem* ExplosionNiagaraSystem;
};