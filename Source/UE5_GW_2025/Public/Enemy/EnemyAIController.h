#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "EnemyAIController.generated.h"

UCLASS()
class UE5_GW_2025_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AEnemyAIController();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

	/** AIの感知コンポーネント */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    TObjectPtr<UAIPerceptionComponent> MyPerceptionComponent;

	// AIの視覚感知設定
    UPROPERTY()
    TObjectPtr<UAISenseConfig_Sight> SightConfig;

	// 感知更新時のイベントハンドラ
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// 最後に感知したプレイヤーの位置
    UPROPERTY()
    FVector LastKnownPlayerLocation;

    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
public:
    // ターゲットの最終位置に移動
    UFUNCTION()
    void MoveToLastKnownLocation();

    /** ターゲット設定 */
    UFUNCTION(BlueprintCallable)
    void SetTarget(APawn* NewTarget);

    /** 移動距離のしきい値 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AcceptanceRadius = 100.0f;

private:
    APawn* TargetPawn;
};
