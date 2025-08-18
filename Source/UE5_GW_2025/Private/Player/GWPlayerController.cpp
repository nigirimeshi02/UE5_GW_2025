// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/GWPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Player/GWPlayer.h"
#include "UI/BulletCounter.h"

void AGWPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 弾数カウンターのUIウィジェットを生成し、画面に追加する
	BulletCounterUI = CreateWidget<UBulletCounter>(this, BulletCounterUIClass);
	BulletCounterUI->AddToPlayerScreen(0);
}

void AGWPlayerController::SetupInputComponent()
{
	// 入力マッピングコンテキストを追加する
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}
}

void AGWPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// ポーンが破壊された時に呼ばれるイベントにバインド
	InPawn->OnDestroyed.AddDynamic(this, &AGWPlayerController::OnPawnDestroyed);

	// プレイヤーキャラクターか確認
	if (AGWPlayer* GWPlayer = Cast<AGWPlayer>(InPawn))
	{
		// プレイヤータグを付与
		GWPlayer->Tags.Add(PlayerPawnTag);

		// 弾数が更新されたイベントにバインド
		GWPlayer->OnMagazineUpdated.AddDynamic(this, &AGWPlayerController::OnBulletCountUpdated);
	}
}

void AGWPlayerController::OnPawnDestroyed(AActor* DestroyedActor)
{
	// プレイヤースタートを探す
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), ActorList);

	if (ActorList.Num() > 0)
	{
		// ランダムなプレイヤースタートを選択
		AActor* RandomPlayerStart = ActorList[FMath::RandRange(0, ActorList.Num() - 1)];

		// 選ばれたプレイヤースタートの位置・回転を取得
		const FTransform SpawnTransform = RandomPlayerStart->GetActorTransform();

		// プレイヤーキャラクターをスポーン
		if (AGWPlayer* RespawnedCharacter = GetWorld()->SpawnActor<AGWPlayer>(CharacterClass, SpawnTransform))
		{
			// 新しいキャラクターを操作対象にする
			Possess(RespawnedCharacter);
		}
	}
}

void AGWPlayerController::OnBulletCountUpdated(int32 MagazineSize, int32 Bullets)
{
	// UIに弾数を反映
	BulletCounterUI->BP_UpdateBulletCount(MagazineSize, Bullets);
}