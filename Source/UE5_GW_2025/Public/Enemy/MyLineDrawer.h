// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/LineBatchComponent.h" // ULineBatchComponentを使用するために必要
#include "MyLineDrawer.generated.h" // クラス名に応じて変更してください

UCLASS()
class UE5_GW_2025_API AMyLineDrawer : public AActor
{
    GENERATED_BODY()

public:    
    AMyLineDrawer();

    /**
     * @brief 始点と終点を結ぶ線（デバッグラインではない）をワールドに描画します。
     * * @param StartLocation 直線の始点のワールド座標 (FVector)
     * @param EndLocation 直線の終点のワールド座標 (FVector)
     * @param LineColor 線の色 (FLinearColor)
     * @param Thickness 線の太さ (float)
     */
    UFUNCTION(BlueprintCallable, Category = "Rendering")
    void DrawPermanentLine(
        const FVector& StartLocation, 
        const FVector& EndLocation, 
        const FLinearColor& LineColor, 
        float Thickness = 5.0f
    );
};
