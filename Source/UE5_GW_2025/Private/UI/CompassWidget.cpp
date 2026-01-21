// CompassWidget.cpp
#include "UI/CompassWidget.h"
#include "Components/Image.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void UCompassWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 必要な参照が揃っているかチェック
    if (!ArrowImage || !GoalActor)
    {
        return;
    }

    // プレイヤーコントローラーを取得
    APlayerController* PC = GetOwningPlayer();
    if (!PC || !PC->GetPawn())
    {
        return;
    }

    // 1. 位置の取得
    FVector PlayerLocation = PC->GetPawn()->GetActorLocation();
    FVector GoalLocation = GoalActor->GetActorLocation();

    // 2. 「見るべき向き」を計算 (Find Look At Rotation)
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, GoalLocation);

    // 3. 「現在のカメラの向き」を取得
    FRotator ControlRotation = PC->GetControlRotation();

    // 4. 差分 (Delta) を計算
    // 単純な引き算の後、Normalize()することで -180 ~ 180 の範囲に正規化されます
    FRotator DeltaRotation = LookAtRotation - ControlRotation;
    DeltaRotation.Normalize();

    // 5. UIの回転を適用 (Z軸 = Yaw)
    ArrowImage->SetRenderTransformAngle(DeltaRotation.Yaw);
}