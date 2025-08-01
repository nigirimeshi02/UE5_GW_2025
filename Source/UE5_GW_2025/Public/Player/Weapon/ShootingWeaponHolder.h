// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ShootingWeaponHolder.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UShootingWeaponHolder : public UInterface
{
	GENERATED_BODY()
};

/**
 * 武器を保持するオブジェクト用のインターフェースクラス
 */
class UE5_GW_2025_API IShootingWeaponHolder
{
	GENERATED_BODY()

public:
	// 武器のメッシュを所持者にアタッチする関数
	virtual void AttachWeaponMeshes(class AShootingWeapon* Weapon) = 0;

	// 武器の発射時のアニメーションモンタージュを再生する関数
	virtual void PlayFiringMontage(UAnimMontage* Montage) = 0;

	// 武器の反動を所持者に加える関数
	virtual void AddWeaponRecoil(float Recoil) = 0;

	// 現在の弾数とマガジンサイズをHUDに反映する関数
	virtual void UpdateWeaponHUD(int32 CurrentAmmo, int32 MagazineSize) = 0;

	// 武器の照準ターゲット位置を計算して返す関数
	virtual FVector GetWeaponTargetLocation() = 0;

	// 指定されたクラスの武器を所持者に追加する関数
	virtual void AddWeaponClass(const TSubclassOf<class AShootingWeapon>& WeaponClass) = 0;

	// 指定された武器が有効化されたときに呼ばれる関数
	virtual void OnWeaponActivated(class AShootingWeapon* Weapon) = 0;

	// 指定された武器が無効化されたときに呼ばれる関数
	virtual void OnWeaponDeactivated(class AShootingWeapon* Weapon) = 0;

	// セミオート武器のクールダウンが終了し再発射可能になったことを通知する関数
	virtual void OnSemiWeaponRefire() = 0;
};
