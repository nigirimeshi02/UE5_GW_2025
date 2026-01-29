#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
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

    FName GetActiveTargetTag() const { return PendingTargetTag; }

private:
    UPROPERTY()
    class UUserWidget* ActiveLoadingWidget;

    // 非同期ロード完了時に呼ばれる関数
    UFUNCTION()
    void OnLevelLoaded();


public:
    // 読み込み完了まで保持しておくタグ名
    UPROPERTY(BlueprintReadOnly, Category = "LevelTransition")
    FName PendingTargetTag;

    UPROPERTY(BlueprintReadOnly, Category = "LevelTransition")
    FName PendingNextLevelName;


    UPROPERTY(BlueprintReadOnly, Category = "LevelTransition")
    FName PendingCurrentLevelName;

protected:
    // 現在再生中のオーディオコンポーネントを保持
    UPROPERTY()
    TObjectPtr<UAudioComponent> CurrentAudioComponent;

    // ★設定用: レベル名とBGM（SoundCue / Wave）の対応リスト
    // エディタ側で「Stage1」→「BGM_Forest」のように設定します
    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TMap<FName, USoundBase*> LevelBGMMap;

public:
    // BGMを再生・切り替える関数
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayLevelBGM(FName LevelName);
};