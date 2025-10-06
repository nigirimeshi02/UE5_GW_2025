// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBar.generated.h"

/**
 * 
 */
UCLASS(abstract)
class UE5_GW_2025_API UHPBar : public UUserWidget
{
	GENERATED_BODY()
	
public:

	// HPバーを更新する
	UFUNCTION(BlueprintImplementableEvent, Category = "HPBar", meta = (DisplayName = "UpdateHPBar"))
	void BP_UpdateHPBar(float MaxHP, float CurrentHP);

};
