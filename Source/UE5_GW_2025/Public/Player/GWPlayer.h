// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Player/Weapon/ShootingWeaponHolder.h"
#include "GWPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMagazineUpdatedDelegate, int32, MagazineSize, int32, Bullets, bool, Infinite);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHPUpdatedDelegate, float, MaxHP, float, CurrentHP);

/**
 * GWプレイヤー
 */
UCLASS()
class UE5_GW_2025_API AGWPlayer : public ACharacter, public IShootingWeaponHolder
{
	GENERATED_BODY()

	// 一人称視点で使用するスケルタルメッシュ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FirstPersonMesh;

	// 一人称カメラコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	// AI感知用ノイズエミッター
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UPawnNoiseEmitterComponent* PawnNoiseEmitter;

protected:
	// ジャンプ用のインプットアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* JumpAction;

	// 移動用のインプットアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	// 視点操作用のインプットアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;

	// 射撃用のインプットアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* FireAction;

	// 武器切り替え用のインプットアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SwitchWeaponAction;

	// リロード用のインプットアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* ReloadAction;

	// ADS用のインプットアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* ADSAction;

	// 一人称武器ソケットの名前
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
	FName FirstPersonWeaponSocket = FName("HandGrip_R");

	// 三人称武器ソケットの名前
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
	FName ThirdPersonWeaponSocket = FName("HandGrip_R");

	// エイム用トレースの最大距離
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aim")
	float MaxAimDistance = 10000.0f;

	// 所持している武器一覧
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
	TArray<class AShootingWeapon*> OwnedWeapons;

	// 現在装備中の武器
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
	TObjectPtr<class AShootingWeapon> CurrentWeapon;

	// 初期武器
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
	TSubclassOf<class AShootingWeapon> InitWeapon;

	// 登り補間にかける時間
	UPROPERTY(EditAnywhere, Category = "Climb")
	float ClimbDuration;

	// 壁方向へ向く速さ
	UPROPERTY(EditAnywhere, Category = "Climb")
	float LookAtInterpSpeed;

	// 登れる高さの上限
	UPROPERTY(EditAnywhere, Category = "Climb")
	float MaxClimbHeight;

	// 壁のぼりのアニメーション
	UPROPERTY(EditDefaultsOnly, Category = "Climb")
	UAnimMontage* ClimbMontage;

	// リロード中？
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "bool")
	bool IsReload;

	// 壁のぼり中
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "bool")
	bool IsClimbing;

	// のぼれる？
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "bool")
	bool CanClimb;

	// リコイルの回復速度
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilRecoverySpeed = 10.0f;

	// 撃った時の瞬間的な揺れ量
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilKickStrength = 1.0f;

	// 壁走り中？
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "bool")
	bool IsWallRunning;

	// 壁走りできる時間
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallRun")
	float MaxWallRunTime = 3.0f;

	// 現在の壁走りしている時間
	UPROPERTY(BlueprintReadOnly, Category = "WallRun")
	float WallRunDuration = 0.0f;

	// 壁走りのクールタイム
	UPROPERTY(EditAnywhere, Category = "WallRun")
	float WallRunCoolTime = 0.5f;

	// 壁走りのスピード
	UPROPERTY(EditAnywhere, Category = "WallRun")
	float WallRunSpeed = 1500.0f;

	// ADS用の値
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ADS")
	float ADSAlpha = 0.0f;

	// ADS中？
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "bool")
	bool IsAiming = false;

	// ADS時のFOV
	UPROPERTY(EditDefaultsOnly, Category = "ADS")
	float ADSFOV = 60.f;

	// ADS時のFOVの補間速度
	UPROPERTY(EditDefaultsOnly, Category = "ADS")
	float ADSFOVInterpSpeed = 15.f;

	// 死亡アニメーション
	UPROPERTY(EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> DeathMontage;

	// 壁走りのSE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* WallRunSound;

	// 壁のぼりのSE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ClimbSound;

public:
	// 向くべき回転値
	FRotator TargetRotation;

	// 壁のぼり壁検知用
	FHitResult ClimbTraceHit;	
	
	// 現在のリコイルオフセット
	float CurrentRecoilPitch = 0.0f;

	// 累積されたリコイル
	float TargetRecoilPitch = 0.0f;

	// 右の壁？
	bool RightWall;

	// 走る壁の法線
	FVector WallRunNormal;

	// 走る壁の方向
	FVector WallRunDirection;

	// 現在の壁走りのクールタイムの時間
	float WallRunCooldownTimer = 0.0f;

	// 壁走り中の視点制限
	float WallRunYawRange = 30.f;     // 左右 ±30度まで

	// 壁走り方向を基準にする角度
	float WallRunTargetYaw = 0.f;

	// 視点を壁方向にスムーズに向ける速度
	float WallRunLookInterpSpeed = 10.f;

	// デフォルトのFOV
	float DefaultFOV = 90.f;

	bool IsDying = false;
	bool IsDeathCameraActive = false;
	float DeathCamTimePassed = 0.0f;
	float DeathCamDuration = 1.0f;
	FVector DeathCamStart;
	FVector DeathCamTarget;
	FRotator DeathCamStartRot;
	FRotator DeathCamTargetRot;

	FTimerHandle RespawnTimerHandle;

public:
	// マガジン更新時のデリゲート
	FMagazineUpdatedDelegate OnMagazineUpdated;

	// HP更新時のデリゲート
	FHPUpdatedDelegate OnHPBarUpdated;

public:
	// コンストラクタ
	AGWPlayer();

protected:
	// ゲーム開始時に一度だけ呼ばれる関数
	virtual void BeginPlay() override;

public:
	// 毎フレーム呼ばれる関数
	virtual void Tick(float DeltaTime) override;

	// インプットアクションのバインディング設定
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// このキャラクターをプレイヤーが操作し始めたときに呼ばれる
	virtual void PossessedBy(AController* NewController) override;

	// アビリティの初期化（GameplayAbilitySystem など）
	void InitializeAbilities();

public:
	// ダメージを受けたときに呼ばれる関数
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// 移動入力処理
	void MoveInput(const FInputActionValue& Value);

	// 視点操作入力処理
	void LookInput(const FInputActionValue& Value);

	// エイム処理（Yaw/Pitch指定）
	virtual void DoAim(float Yaw, float Pitch);

	// 移動処理（方向指定）
	virtual void DoMove(float Right, float Forward);

	// ジャンプ開始処理
	virtual void DoJumpStart();

	// ジャンプ終了処理
	virtual void DoJumpEnd();

	// 射撃開始処理
	void DoStartFiring();

	// 射撃停止処理
	void DoStopFiring();

	// 武器切り替え処理
	void DoSwitchWeapon();

	// リロード開始処理
	void DoReloadStart();

	// リロード終了処理
	void DoReloadEnd(class UAnimMontage* Montage, bool bInterrupted);

	// 壁のぼりを試みる
	void TryStartClimb();

	// 壁のぼり開始処理
	void DoStartClimb(const FHitResult& Hit);

	// 壁のぼり終了処理
	void DoEndClimb();

	// 壁を確認する
	bool CheckClimb(FHitResult& OutHit);

	// 壁走り開始処理
	void DoStartWallRun(const FVector& WallNormal, bool RightSide);

	// 壁走り終了処理
	void DoEndWallRun();

	// 壁走りできるか確認
	bool TraceWall(FVector& OutWallNormal, bool RightSide);

	// 壁走りのチェック
	void CheckWallRun();

	// 壁走り中の視点制限
	void LimitWallRunCamera(float Value);

	// ADS開始処理
	void DoStartADS();

	// ADS終了処理
	void DoEndADS();

	// 死亡処理
	void OnDeath();

	// 死亡時のカメラ演出開始処理
	void DoStartDeathCamera();

	// 死亡時のカメラ演出終了処理
	void DoFinishDeathCamera();

	// リスポーンするための処理
	void Respawn();

public:
	// 武器のメッシュをキャラクターにアタッチする処理
	virtual void AttachWeaponMeshes(class AShootingWeapon* Weapon) override;

	// 射撃アニメーションモンタージュを再生
	virtual void PlayFiringMontage(UAnimMontage* Montage) override;

	// リロードアニメーションモンタージュを再生
	virtual void PlayReloadMontage(UAnimMontage* Montage) override;

	// 射撃による反動を適用
	virtual void AddWeaponRecoil(float Recoil) override;

	// HUDに現在の弾数情報を更新
	virtual void UpdateWeaponHUD(int32 CurrentAmmo, int32 MagazineSize, bool Infinite) override;

	// 武器用の照準位置を取得
	virtual FVector GetWeaponTargetLocation() override;

	// 指定クラスの武器を追加
	virtual void AddWeaponClass(const TSubclassOf<class AShootingWeapon>& WeaponClass) override;

	// 武器がアクティブになったときの処理
	virtual void OnWeaponActivated(class AShootingWeapon* Weapon) override;

	// 武器が非アクティブになったときの処理
	virtual void OnWeaponDeactivated(class AShootingWeapon* Weapon) override;

	// セミオート武器の再発射可能通知
	virtual void OnSemiWeaponRefire() override;

	// リロード
	virtual void Reload()override;

	// 武器を追加できるか確認
	virtual bool CheckAddWeapon()override;

	// 指定された武器を削除する
	virtual bool RemoveWeapon(class AShootingWeapon* Weapon)override;

	// 弾薬の追加
	virtual void AddAmmo()override;

	// 弾薬の追加ができるか確認する
	virtual bool CheckAddAmmo() override;

protected:
	// 指定クラスの武器をすでに所持しているかを確認
	AShootingWeapon* FindWeaponOfType(TSubclassOf<AShootingWeapon> WeaponClass) const;

public:
	// HUDに現在のHP情報を更新
	void UpdateHPHUD(int32 CurrentHP, int32 MaxHP);

	// 初期武器の追加
	void AddWeaponInit();

public:
	// 一人称メッシュを取得
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	// 一人称カメラを取得
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintPure)
	AShootingWeapon* GetCurrentWeapon()const { return CurrentWeapon; }


};
