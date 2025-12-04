// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmmoSymbol.generated.h"

UCLASS()
class UE5_GW_2025_API AAmmoSymbol : public AActor
{
	GENERATED_BODY()

protected:
	// 衝突判定用のスフィアコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereCollision;

	// 武器ピックアップの表示メッシュ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;
	
public:	
	// コンストラクタ
	AAmmoSymbol();

protected:
	// ゲーム開始時に呼ばれる初期化処理
	virtual void BeginPlay() override;

	// 衝突判定の重なりを処理する関数
	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// 毎フレーム呼ばれる処理
	virtual void Tick(float DeltaTime) override;

};
