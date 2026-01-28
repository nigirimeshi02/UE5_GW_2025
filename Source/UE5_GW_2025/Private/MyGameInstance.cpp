#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Player/GWPlayerController.h"

void UMyGameInstance::ChangeLevelAsync(FName NextLevelName, FName CurrentLevelName, FName TargetTag)
{
    // --- 時間の流れを元に戻す ---
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

    PendingTargetTag = TargetTag;
    PendingNextLevelName = NextLevelName;

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
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    AGWPlayerController* PC = Cast<AGWPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    APlayerStart* TargetStart = nullptr;

    // 1. まず、指定されたタグを持つ PlayerStart を「最初に」検索する
    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        if (It && It->PlayerStartTag == PendingTargetTag)
        {
            TargetStart = *It;
            break;
        }
    }

    if (PC)
    {
        // ★タイトル画面やゲームオーバーへの遷移
        if (PendingNextLevelName == FName("LVL_Title") || PendingNextLevelName == FName("LVL_GameOver"))
        {
            PC->SetGameplayUIVisible(false);
            PC->bShowMouseCursor = true;
            PC->SetInputMode(FInputModeUIOnly());
        }
        // ★通常のステージへの遷移
        else
        {
            PC->SetGameplayUIVisible(true);
            PC->bShowMouseCursor = false;
            PC->SetInputMode(FInputModeGameOnly());

            // 2. プレイヤーを移動させ、向きを固定する
            if (TargetStart && PlayerPawn)
            {
                // ポーンの位置と向きをセット
                PlayerPawn->SetActorLocationAndRotation(
                    TargetStart->GetActorLocation(),
                    TargetStart->GetActorRotation(),
                    false,
                    nullptr,
                    ETeleportType::TeleportPhysics
                );

                // 【重要】コントローラーの向きを PlayerStart の向きに合わせる
                // これをしないと、カメラの向きが前のレベルのままになります
                PC->SetControlRotation(TargetStart->GetActorRotation());

                // カメラの注視点を更新
                PC->SetViewTarget(PlayerPawn);
            }
        }
    }

    // 3. ロード画面を非表示
    if (ActiveLoadingWidget)
    {
        ActiveLoadingWidget->RemoveFromParent();
        ActiveLoadingWidget = nullptr;
    }
}