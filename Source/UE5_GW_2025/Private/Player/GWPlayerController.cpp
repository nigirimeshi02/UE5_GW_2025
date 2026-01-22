// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/GWPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Player/GWPlayer.h"
#include "UI/BulletCounter.h"
#include "UI/HPBar.h"
#include "Abilities/PlayerAttributeSet.h"
#include "Player/GWPlayerState.h"
#include "Player/GWPlayerCameraManager.h"
#include "MyGameInstance.h"

AGWPlayerController::AGWPlayerController()
{
	PlayerCameraManagerClass = AGWPlayerCameraManager::StaticClass();
}

void AGWPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 弾数カウンターのUIウィジェットを生成し、画面に追加する
	BulletCounterUI = CreateWidget<UBulletCounter>(this, BulletCounterUIClass);
	BulletCounterUI->AddToPlayerScreen(0);
	BulletCounterUI->SetVisibility(ESlateVisibility::Collapsed);

	// HPバーのUIウィジェットを作成し、画面に追加する
	HPBarUI = CreateWidget<UHPBar>(this, HPBarUIClass);
	HPBarUI->AddToPlayerScreen(1);
	HPBarUI->SetVisibility(ESlateVisibility::Collapsed);

	// 現在の HP を取得
	AGWPlayerState* GWPS = Cast<AGWPlayerState>(PlayerState);
	if (GWPS)
	{
		UPlayerAttributeSet* AttributeSet = GWPS->GetAttributeSet();
		if (HPBarUI)
		{
			HPBarUI->BP_UpdateHPBar(AttributeSet->GetMaxHealth(), AttributeSet->GetHealth());
		}
		if (AGWPlayer* GWPlayer = Cast<AGWPlayer>(GetPawn()))
		{
			GWPlayer->AddWeaponInit();
		}
	}

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

		// HPバーが更新されたイベントにバインド
		GWPlayer->OnHPBarUpdated.AddDynamic(this, &AGWPlayerController::OnHPBarUpdated);

		// 現在の HP を取得
		AGWPlayerState* GWPS = Cast<AGWPlayerState>(PlayerState);
		if (GWPS)
		{
			// BeginPlayよりも先にここが呼ばれるため最初はnull
			// 以下の処理はポーンが切り替わった時用
			if (HPBarUI)
			{
				UPlayerAttributeSet* AttributeSet = GWPS->GetAttributeSet();
				HPBarUI->BP_UpdateHPBar(AttributeSet->GetMaxHealth(), AttributeSet->GetHealth());
				GWPlayer->AddWeaponInit();
			}
		}
	}
}

void AGWPlayerController::OnPawnDestroyed(AActor* DestroyedActor)
{
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
	if (!GI) return;

	// 現在のステージに設定されているタグを取得
	FName CurrentStageTag = GI->GetActiveTargetTag();

	// 1. 全世界の PlayerStart を一旦取得
	TArray<AActor*> AllPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllPlayerStarts);

	// 2. 現在のステージに属する（タグが一致する）PlayerStart だけを抽出
	TArray<APlayerStart*> ValidStarts;
	for (AActor* Actor : AllPlayerStarts)
	{
		APlayerStart* PS = Cast<APlayerStart>(Actor);
		// MyGameInstance で指定したタグと一致するものだけを候補に入れる
		if (PS && PS->PlayerStartTag == CurrentStageTag)
		{
			ValidStarts.Add(PS);
		}
	}

	// 3. 有効な開始地点が見つかった場合のみリスポーン
	if (ValidStarts.Num() > 0)
	{
		// 現在のレベル内の候補からランダムに選択
		APlayerStart* TargetStart = ValidStarts[FMath::RandRange(0, ValidStarts.Num() - 1)];
		const FTransform SpawnTransform = TargetStart->GetActorTransform();

		if (AGWPlayer* RespawnedCharacter = GetWorld()->SpawnActor<AGWPlayer>(CharacterClass, SpawnTransform))
		{
			Possess(RespawnedCharacter);

			// 重要：カメラを新しいキャラクターに向ける
			SetViewTarget(RespawnedCharacter);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No PlayerStart found with tag: %s"), *CurrentStageTag.ToString());
	}
}

void AGWPlayerController::OnBulletCountUpdated(int32 MagazineSize, int32 Bullets, bool Infinite)
{
	// UIに弾数を反映
	BulletCounterUI->BP_UpdateBulletCount(MagazineSize, Bullets, Infinite);
}

void AGWPlayerController::OnHPBarUpdated(float MaxHP, float CurrentHP)
{
	// UIにHPを反映
	HPBarUI->BP_UpdateHPBar(MaxHP, CurrentHP);
}

void AGWPlayerController::SetGameplayUIVisible(bool bVisible)
{
	// 表示なら HitTestInvisible (または Visible)、非表示なら Collapsed
	ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;

	if (BulletCounterUI)
	{
		BulletCounterUI->SetVisibility(NewVisibility);
	}

	if (HPBarUI)
	{
		HPBarUI->SetVisibility(NewVisibility);

		// 表示に切り替わったタイミングで最新の値を反映
		if (bVisible)
		{
			AGWPlayerState* GWPS = Cast<AGWPlayerState>(PlayerState);
			if (GWPS)
			{
				UPlayerAttributeSet* AttributeSet = GWPS->GetAttributeSet();
				HPBarUI->BP_UpdateHPBar(AttributeSet->GetMaxHealth(), AttributeSet->GetHealth());
			}
		}
	}
}
