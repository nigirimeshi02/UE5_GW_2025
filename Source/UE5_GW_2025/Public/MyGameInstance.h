#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "MyGameInstance.generated.h"

UCLASS()
class UE5_GW_2025_API UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    // 次のレベル名、現在のレベル名、移動先のPlayerStartタグを指定して遷移を開始
    UFUNCTION(BlueprintCallable, Category = "LevelTransition")
    void ChangeLevelAsync(FName NextLevelName, FName CurrentLevelName, FName TargetTag);

    // ロード画面用Widget（エディタのBP側から指定）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UUserWidget> LoadingWidgetClass;

private:
    UPROPERTY()
    class UUserWidget* ActiveLoadingWidget;

    // 非同期ロード完了時に呼ばれる関数
    UFUNCTION()
    void OnLevelLoaded();

    // 読み込み完了まで保持しておくタグ名
    FName PendingTargetTag;

    FName PendingNextLevelName;
};