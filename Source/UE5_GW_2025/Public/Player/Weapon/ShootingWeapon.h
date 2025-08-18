// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShootingWeapon.generated.h"

UCLASS()
class UE5_GW_2025_API AShootingWeapon : public AActor
{
	GENERATED_BODY()

	// 一人称視点メッシュ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FirstPersonMesh;

	// 三人称視点メッシュ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* ThirdPersonMesh;

protected:

	// 武器の所有者
	class IShootingWeaponHolder* WeaponOwner;

	// 弾のクラス
	UPROPERTY(EditAnywhere, Category = "Ammo")
	TSubclassOf<class AShootingProjectile> ProjectileClass;

	// マガジン内の弾の数
	UPROPERTY(EditAnywhere, Category = "Ammo")
	int32 MagazineSize = 10;

	// 現在のマガジン内の弾の数
	int32 CurrentBullets = 0;

	// 銃を撃つときのアニメーション
	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* FiringMontage;

	// リロードのときのアニメーション
	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* ReloadMontage;

	// 武器が使えるときの一人称視点メッシュのAnimInstanceクラス
	UPROPERTY(EditAnywhere, Category = "Animation")
	TSubclassOf<class UAnimInstance> FirstPersonAnimInstanceClass;

	// 武器が使えるときの三人称視点メッシュのAnimInstanceクラス
	UPROPERTY(EditAnywhere, Category = "Animation")
	TSubclassOf<class UAnimInstance> ThirdPersonAnimInstanceClass;

	// 照準ブレ
	UPROPERTY(EditAnywhere, Category = "Aim")
	float AimVariance = 0.0f;

	// 銃の反動
	UPROPERTY(EditAnywhere, Category = "Aim")
	float FiringRecoil = 0.0f;

	// 銃のマズルのソケット名
	UPROPERTY(EditAnywhere, Category = "Aim")
	FName MuzzleSocketName;

	// 銃口から弾丸が出現する距離
	UPROPERTY(EditAnywhere, Category = "Aim")
	float MuzzleOffset = 10.0f;

	// フルオートフラグ
	UPROPERTY(EditAnywhere, Category = "Refire")
	bool bFullAuto = false;

	// 武器の射撃間隔
	UPROPERTY(EditAnywhere, Category = "Refire")
	float RefireRate = 0.5f;

	// 前回の射撃時間
	float TimeOfLastShot = 0.0f;

	// 発射フラグ
	bool bIsFiring = false;

	// フルオート再発射を制御するタイマー
	FTimerHandle RefireTimer;

	// ポーンの所有者
	TObjectPtr<APawn> PawnOwner;

	// AI認識システムのインタラクションにおけるショットの音量
	UPROPERTY(EditAnywhere, Category = "Perception")
	float ShotLoudness = 1.0f;

	// 射撃AI認識ノイズの最大範囲
	UPROPERTY(EditAnywhere, Category = "Perception")
	float ShotNoiseRange = 3000.0f;

	// この武器を撃ったときに発生するノイズに適用するタグ
	UPROPERTY(EditAnywhere, Category = "Perception")
	FName ShotNoiseTag = FName("Shot");
	
public:	
	// コンストラクタ
	AShootingWeapon();

protected:
	// ゲーム開始時に一度だけ呼ぶ関数
	virtual void BeginPlay() override;

	// ゲーム終了時に一度だけ呼ぶ関数
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
public:	
	// 毎フレーム呼ぶ関数
	virtual void Tick(float DeltaTime) override;

protected:
	// 所有者が破壊されたときに呼ばれる関数
	UFUNCTION()
	void OnOwnerDestroyed(AActor* DestroyedActor);

public:
	// 武器を使えるようにする
	void ActivateWeapon();

	// 武器を使えないようにする
	void DeactivateWeapon();

	// 射撃開始
	void StartFiring();

	// 射撃終了
	void StopFiring();

	// リロード
	void Reload();

protected:
	// 発砲処理
	virtual void Fire();

	// セミオートの射撃中に再発射の時間が経過したときに呼び出される
	void FireCooldownExpired();

	// 弾の発射処理
	virtual void FireProjectile(const FVector& TargetLocation);

	// 弾の発射位置計算処理
	FTransform CalculateProjectileSpawnTransform(const FVector& TargetLocation) const;

public:
	// FirstPersonMeshを取得
	UFUNCTION(BlueprintPure, Category = "Weapon")
	class USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; };

	// ThirdPersonMeshを取得
	UFUNCTION(BlueprintPure, Category = "Weapon")
	class USkeletalMeshComponent* GetThirdPersonMesh() const { return ThirdPersonMesh; };

	// FirstPersonAnimInstanceClassを取得
	const TSubclassOf<class UAnimInstance>& GetFirstPersonAnimInstanceClass() const;

	// ThirdPersonAnimInstanceClassを取得
	const TSubclassOf<class UAnimInstance>& GetThirdPersonAnimInstanceClass() const;

	// マガジン内の弾の数を取得
	int32 GetMagazineSize() const { return MagazineSize; };

	// 現在のマガジン内の弾の数を取得
	int32 GetBulletCount() const { return CurrentBullets; }

};
