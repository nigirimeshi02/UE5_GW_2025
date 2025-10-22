#include "Enemy/MyLineDrawer.h"
#include "Engine/World.h"
#include "Components/LineBatchComponent.h"

AMyLineDrawer::AMyLineDrawer()
{
    // Tickを有効にする必要があればここで設定
    PrimaryActorTick.bCanEverTick = true;
}

void AMyLineDrawer::DrawPermanentLine(
    const FVector& StartLocation,
    const FVector& EndLocation,
    const FLinearColor& LineColor,
    float Thickness
)
{
    UWorld* World = GetWorld();

    if (World)
    {
        // ワールドからULineBatchComponentを取得
        ULineBatchComponent* LineBatcher = World->GetLineBatcher(UWorld::ELineBatcherType::World);

        // LineBatcherが有効か確認
        if (LineBatcher)
        {
            /**
             * ULineBatchComponent::DrawLine 関数
             * LineBatcherに線を追加し、毎フレーム自動的に描画されます。
             * * パラメータ:
             * 1. const FVector& Start: 始点
             * 2. const FVector& End: 終点
             * 3. const FLinearColor& Color: 線の色
             * 4. ESceneDepthPriorityGroup DepthPriority: 深度優先度 (通常はSDPG_World)
             * 5. float Thickness: 線の太さ
             * 6. float LifeTime: 表示時間（秒）。0.fを設定すると、コンポーネントがクリアされるまで毎フレーム描画されます。
             */
            LineBatcher->DrawLine(
                StartLocation,
                EndLocation,
                LineColor,
                SDPG_World,    // ワールド内の通常オブジェクトと同じ深度
                Thickness,
                0.5f            // ライフタイムを0にすることで、永続的に描画キューに残ります
            );

            // 重要: LineBatcherは毎フレーム描画キューをクリアする設定になっていることが多いため、
            // 永続的に描画し続けるには、この関数を毎フレーム（例: Tick関数内）呼び出す必要があります。
            // あるいは、LineBatcherがクリアされない設定になっていることを確認する必要があります。
        }
    }
}