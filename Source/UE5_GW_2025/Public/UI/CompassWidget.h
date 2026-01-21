// CompassWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CompassWidget.generated.h"

// 前方宣言
class UImage;

UCLASS()
class UE5_GW_2025_API UCompassWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // ゴールとなるアクタ（BPから設定可能にする）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compass")
    AActor* GoalActor;

protected:
    // UMG上のImageウィジェットと紐付ける変数
    // UMG側でも必ず "ArrowImage" という名前で配置する必要があります
    UPROPERTY(meta = (BindWidget))
    UImage* ArrowImage;

    // 毎フレームの更新処理（BPのEvent Tickに相当）
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};