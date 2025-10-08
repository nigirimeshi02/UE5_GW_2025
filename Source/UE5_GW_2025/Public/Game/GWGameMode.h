// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GWGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UE5_GW_2025_API AGWGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// 残機
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rule")
	int32 PlayerLife;

public:
	// コンストラクタ
	AGWGameMode();
	
	// 残機を減らす関数
	UFUNCTION(BlueprintCallable, Category = "Game Rule")
	void DecreaseLife();

	// ゲームオーバーのときに呼ばれる関数
	UFUNCTION(BlueprintCallable, Category = "Game Rule")
	void OnGameOver();

};
