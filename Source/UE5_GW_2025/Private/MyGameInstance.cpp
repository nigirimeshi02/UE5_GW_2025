#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

void UMyGameInstance::ChangeLevelAsync(FName NextLevelName, FName CurrentLevelName, FName TargetTag)
{
    PendingTargetTag = TargetTag;

    // 1. ロード画面を表示
    if (LoadingWidgetClass && !ActiveLoadingWidget)
    {
        ActiveLoadingWidget = CreateWidget<UUserWidget>(this, LoadingWidgetClass);
        if (ActiveLoadingWidget)
        {
            ActiveLoadingWidget->AddToViewport(100);
        }
    }

    // 2. 現在のレベルをアンロード（不要な場合は飛ばしても可）
    if (!CurrentLevelName.IsNone())
    {
        FLatentActionInfo UnloadInfo;
        UGameplayStatics::UnloadStreamLevel(this, CurrentLevelName, UnloadInfo, false);
    }

    // 3. 次のレベルを非同期で読み込み
    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.ExecutionFunction = FName("OnLevelLoaded");
    LatentInfo.UUID = FMath::Rand(); // 実行ごとにユニークなIDを付与
    LatentInfo.Linkage = 0;

    // 第3引数(bMakeVisibleAfterLoad)を true にして読み込み完了後に表示
    UGameplayStatics::LoadStreamLevel(this, NextLevelName, true, false, LatentInfo);
}

void UMyGameInstance::OnLevelLoaded()
{
    APlayerStart* TargetStart = nullptr;
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0); // 追加

    // 1. 指定されたタグを持つ PlayerStart を検索 (前回と同じ)
    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        APlayerStart* FoundStart = *It;
        if (FoundStart && FoundStart->PlayerStartTag == PendingTargetTag)
        {
            TargetStart = FoundStart;
            break;
        }
    }

    // 2. プレイヤーの移動とカメラのリセット
    if (TargetStart && PlayerPawn && PC)
    {
        // 位置を移動
        PlayerPawn->SetActorLocationAndRotation(
            TargetStart->GetActorLocation(),
            TargetStart->GetActorRotation(),
            false,
            nullptr,
            ETeleportType::TeleportPhysics
        );

    }

    // 3. ロード画面を非表示 (前回と同じ)
    if (ActiveLoadingWidget)
    {
        ActiveLoadingWidget->RemoveFromParent();
        ActiveLoadingWidget = nullptr;
    }
}