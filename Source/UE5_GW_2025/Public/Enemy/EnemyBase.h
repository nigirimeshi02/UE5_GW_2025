#pragma once

#include "EnemyState.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"


class UEnemyStateMachineComponent;
class UEnemyWeaponComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDied);

UCLASS()
class UE5_GW_2025_API AEnemyBase : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyBase();

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // ステートマシンコンポーネント
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
    TObjectPtr<UEnemyStateMachineComponent> StateMachine;
	// 武器コンポーネント
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
    TObjectPtr<UEnemyWeaponComponent> WeaponComponent;
	// プレイヤーとどこまで近づけるか
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
    float AcceptanceRadius = 100.0f; // デフォルト値
    // 敵の体力
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy|Stats")
    float MaxHealth = 250.0f;
	// 現在の体力
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Stats")
    float CurrentHealth;

    // --- 弱点コリジョン ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeakPoint")
    USphereComponent* WeakPointSphere;

    // --- 弱点ヒット時の倍率 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeakPoint")
    float WeakPointDamageMultiplier = 2.0f;

    // --- 弱点ヒット判定半径（自動設定されますが変更可能） ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeakPoint")
    float WeakPointRadius = 50.0f;

    // どのボーンにアタッチするか（例："head"）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeakPoint")
    FName WeakPointBoneName = "head";

    /**
     * ダメージを受けたときに表示するウィジェットのブループリントクラス
     * 例: ダメージ数値、ヒットインジケーターなど
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> DamageWidgetClass;

    /**
     * ウィジェットが表示される時間（秒）
     * この時間が経過するとウィジェットは自動的に削除されます
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    float WidgetDisplayDuration = 0.1f;

public:
    virtual void Tick(float DeltaTime) override;

    // プレイヤーを発見したとき呼ばれる
    UFUNCTION(BlueprintCallable)
    virtual void OnPlayerSpotted(APawn* PlayerPawn);

    // プレイヤーをロストしたとき呼ばれる
    UFUNCTION(BlueprintCallable)
    virtual void OnPlayerLost();

    UFUNCTION(BlueprintCallable)
    virtual void AttackTarget();

    UFUNCTION(BlueprintCallable)
    virtual void MoveToTarget(APawn* Target);

	// ステートマシンの状態を更新(idle)
    void OnSearchComplete();

	// 現在のステートを取得
    EEnemyState GetCurrentState() const;

    // ダメージを受けた時
    virtual float TakeDamage(
        float DamageAmount,
        struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator,
        AActor* DamageCauser
    ) override;

    // 死亡処理
    virtual void Die();

	// 敵が死亡のイベント
    UPROPERTY(BlueprintAssignable, Category = "Enemy")
    FOnEnemyDied OnEnemyDied;

};
