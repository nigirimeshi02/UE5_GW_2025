// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Engine/StaticMesh.h"
#include "ShootingWeaponSymbol.generated.h"

/**
 * 武器シンボルの種類に関する情報を保持する構造体
 */
USTRUCT(BlueprintType)
struct FWeaponTable : public FTableRowBase
{
	GENERATED_BODY()

	// ピックアップ時に表示するメッシュ
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> StaticMesh;

	// ピックアップ時に生成・付与される武器のクラス
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AShootingWeapon> WeaponToSpawn;
};


UCLASS()
class UE5_GW_2025_API AShootingWeaponSymbol : public AActor
{
	GENERATED_BODY()

	// 衝突判定用のスフィアコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereCollision;

	// 武器ピックアップの表示メッシュ（武器データテーブルから設定される）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;

protected:

	// このピックアップが表す武器タイプおよび表示内容（データテーブル経由で設定）
	UPROPERTY(EditAnywhere, Category = "Pickup")
	FDataTableRowHandle WeaponType;

	// ピックアップ時に付与される武器クラス（データテーブルから取得される） 
	TSubclassOf<class AShootingWeapon> WeaponClass;

	// このピックアップがリスポーンするまでの待ち時間（秒）
	UPROPERTY(EditAnywhere, Category = "Pickup")
	float RespawnTime = 4.0f;

	// リスポーン用タイマー
	FTimerHandle RespawnTimer;

public:
	// このアクターのデフォルト値を設定する
	AShootingWeaponSymbol();

protected:
	// コンストラクションスクリプト（エディタ上やスポーン時に呼ばれる）
	virtual void OnConstruction(const FTransform& Transform) override;

	// ゲーム開始時に呼ばれる初期化処理
	virtual void BeginPlay() override;

	// 終了処理（アクターの破棄など）
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 衝突判定の重なりを処理する関数
	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// 毎フレーム呼ばれる処理
	virtual void Tick(float DeltaTime) override;

protected:

	// ピックアップがリスポーンするタイミングで呼ばれる関数
	void RespawnPickup();

	// リスポーン演出をBlueprint側に委ねる（演出終了時に FinishRespawn を呼ぶ必要あり）
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup", meta = (DisplayName = "OnRespawn"))
	void BP_OnRespawn();

	// リスポーン後、このピックアップを有効化する 
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void FinishRespawn();

};
