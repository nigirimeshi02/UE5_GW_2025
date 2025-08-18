// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GWPlayerController.generated.h"

/**
 * プレイヤーの入力処理や UI 管理、ポーンの管理を行うコントローラクラス
 */
UCLASS(abstract)
class UE5_GW_2025_API AGWPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	// このプレイヤーに適用する入力マッピングコンテキストのリスト
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<class UInputMappingContext*> DefaultMappingContexts;

	// ポーンが破壊された際にリスポーンさせるキャラクタークラス
	UPROPERTY(EditAnywhere, Category = "Input")
	TSubclassOf<class AGWPlayer> CharacterClass;

	// 弾数を表示する UI ウィジェットのクラス
	UPROPERTY(EditAnywhere, Category = "Input")
	TSubclassOf<class UBulletCounter> BulletCounterUIClass;

	// ポーンに付与して「プレイヤー」であることを識別するためのタグ
	UPROPERTY(EditAnywhere, Category = "Input")
	FName PlayerPawnTag = FName("Player");

	// 生成された弾数カウンター UI の参照
	TObjectPtr<class UBulletCounter> BulletCounterUI;

protected:

	// ゲーム開始時に呼ばれる初期化処理
	virtual void BeginPlay() override;

	// 入力コンポーネントの初期化処理（キーマッピングや入力イベントの設定）
	virtual void SetupInputComponent() override;

	// ポーンを所持したときに呼ばれる処理（キャラ初期化など）
	virtual void OnPossess(APawn* InPawn) override;

	// 所持しているポーンが破壊されたときに呼ばれる処理
	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedActor);

	// 所持ポーンの弾数が更新されたときに呼ばれる処理
	UFUNCTION()
	void OnBulletCountUpdated(int32 MagazineSize, int32 Bullets);

};