// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GWGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameInstance.h"
#include "Engine/LevelStreaming.h" // ★これが必要です
#include "Misc/PackageName.h"      // パスから短い名前を取り出すのに必要
#include "Kismet/GameplayStatics.h"
#include "Player/GWPlayer.h"

AGWGameMode::AGWGameMode()
{
	PlayerLife = 3;
}

void AGWGameMode::DecreaseLife()
{
	PlayerLife--;

	if (PlayerLife <= 0)
	{
		OnGameOver();
	}
}

void AGWGameMode::OnGameOver()
{
	//UGameplayStatics::OpenLevel(this, TEXT("LVL_GameOver"));

    // --- 1. 現在表示されているストリーミングレベルを探す ---
    FName CurrentLevelName = NAME_None;

    // ワールド内の全サブレベルを取得
    const TArray<ULevelStreaming*>& StreamedLevels = GetWorld()->GetStreamingLevels();

    for (ULevelStreaming* StreamingLevel : StreamedLevels)
    {
        // 「ロード済み」かつ「可視状態（Visible）」のレベルを探す
        if (StreamingLevel && StreamingLevel->IsLevelLoaded() && StreamingLevel->IsLevelVisible())
        {
            // パッケージ名から短い名前（L_Stage01など）を取得
            FString PackageName = StreamingLevel->GetWorldAssetPackageName();
            CurrentLevelName = FName(*FPackageName::GetShortName(PackageName));

            // 見つかったらループ終了
            break;
        }
    }

	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
	if (GI)
	{
        PlayerLife = 3;

        AGWPlayer* GP = Cast<AGWPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
        GP->Initialize();

		// 引数: (次のレベル, 今のレベル, 次のレベルのスタート地点タグ)
		GI->ChangeLevelAsync(TEXT("LVL_GameOver"), CurrentLevelName, TEXT("GameOver"));

	}
}
