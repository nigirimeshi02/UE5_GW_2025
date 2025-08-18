// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShootingProjectile.generated.h"

UCLASS()
class UE5_GW_2025_API AShootingProjectile : public AActor
{
	GENERATED_BODY()

	// 弾の衝突判定を行うコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionComponent;

	// 弾の移動処理を行うコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

protected:

	// 命中時にAI感知用のノイズとして扱われる音の大きさ
	UPROPERTY(EditAnywhere, Category = "Noise")
	float NoiseLoudness = 3.0f;

	// 命中時にAIが感知できるノイズの範囲（半径）
	UPROPERTY(EditAnywhere, Category = "Noise")
	float NoiseRange = 3000.0f;

	// ノイズに付与されるタグ（AI感知用）
	UPROPERTY(EditAnywhere, Category = "Noise")
	FName NoiseTag = FName("Projectile");

	// 命中時に加える物理的な力の大きさ
	UPROPERTY(EditAnywhere, Category = "Hit")
	float PhysicsForce = 100.0f;

	// 命中時に与える基礎ダメージ量
	UPROPERTY(EditAnywhere, Category = "Hit")
	float HitDamage = 25.0f;

	// ダメージの種類（火炎や爆発など特定のダメージタイプに対応）
	UPROPERTY(EditAnywhere, Category = "Hit")
	TSubclassOf<UDamageType> HitDamageType;

	// この弾がすでに何かに命中したかどうかのフラグ
	bool bHit = false;

public:
	// このアクターのデフォルト値を設定
	AShootingProjectile();

protected:
	// ゲーム開始時またはスポーン時に呼ばれる関数
	virtual void BeginPlay() override;

	// 衝突時に呼ばれる関数
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

public:
	// 毎フレーム呼ばれる関数
	virtual void Tick(float DeltaTime) override;

protected:
	// キャラクターにダメージを与える処理
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	virtual void DamageCharacter(ACharacter* HitCharacter, const FHitResult& Hit);

	// 弾が命中した際にブループリントでエフェクト等を処理するイベント（実装はBlueprint側
	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile", meta = (DisplayName = "On Projectile Hit"))
	void BP_OnProjectileHit(const FHitResult& Hit);
};
