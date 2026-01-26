#include "UI/CompassWidget.h"
#include "Components/Image.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "EngineUtils.h"
#include "MyGameInstance.h"
#include "Objects/GoalActor.h"
#include "Misc/PackageName.h" // GetShortNameを使うために必要

void UCompassWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
    if (!ArrowImage || !GI) return;

    // 1. GoalActor が無効なら再検索
    if (!IsValid(GoalActor))
    {
        for (TActorIterator<AGoalActor> It(GetWorld()); It; ++It)
        {
            AGoalActor* FoundActor = *It;
            if (IsValid(FoundActor))
            {
                // --- 修正ポイント：Package から直接名前を取得 ---
                // GetPackage() はそのアクターが保存されているファイル（パッケージ）を返します
                UPackage* ActorPackage = FoundActor->GetPackage();
                if (!ActorPackage) continue;

                // パッケージ名（例: /Game/Maps/Stage1）を取得し、ショートネーム（Stage1）にする
                FString FullPath = ActorPackage->GetName();
                FString ActorLevelName = FPackageName::GetShortName(FullPath);
                FString PureLevelName = UWorld::RemovePIEPrefix(ActorLevelName);

                FString TargetLevelName = GI->PendingNextLevelName.ToString();

                // ログで確認用（親の名前が出ていないかチェック）
                UE_LOG(LogTemp, Log, TEXT("Comparing: ActorLevel[%s] vs Target[%s]"), *PureLevelName, *TargetLevelName);

                if (PureLevelName == TargetLevelName)
                {
                    GoalActor = FoundActor;
                    break;
                }
            }
        }
    }

    // ゴールが見つかっていない、または非表示設定なら終了
    if (!IsValid(GoalActor) || GetVisibility() != ESlateVisibility::HitTestInvisible)
    {
        return;
    }

    // 2. プレイヤー情報の取得
    APlayerController* PC = GetOwningPlayer();
    if (!PC || !PC->GetPawn()) return;

    // 3. 回転計算
    FVector PlayerLocation = PC->GetPawn()->GetActorLocation();
    FVector GoalLocation = GoalActor->GetActorLocation();

    // Z軸（高さ）の差を無視して水平方向の向きだけ計算
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, GoalLocation);
    FRotator ControlRotation = PC->GetControlRotation();

    FRotator DeltaRotation = LookAtRotation - ControlRotation;
    DeltaRotation.Normalize();

    // 4. 回転の適用
    ArrowImage->SetRenderTransformAngle(DeltaRotation.Yaw);
}