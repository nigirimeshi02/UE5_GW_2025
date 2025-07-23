// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GWPlayer.generated.h"

UCLASS()
class UE5_GW_2025_API AGWPlayer : public ACharacter
{
	GENERATED_BODY()

	// 一人称メッシュ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FirstPersonMesh;

	// 一人称カメラ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

protected:
	// ジャンプアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* JumpAction;

	// 移動アクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	// 視点操作アクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;


public:
	// コンストラクタ
	AGWPlayer();

protected:
	// ゲーム開始時に一度だけ呼ぶ関数
	virtual void BeginPlay() override;

public:	
	// 毎フレーム呼ぶ関数
	virtual void Tick(float DeltaTime) override;

	// インプットアクションのバインドを設定する
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	// 移動入力
	void MoveInput(const FInputActionValue& Value);

	// 視点操作入力
	void LookInput(const FInputActionValue& Value);

	// エイム
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoAim(float Yaw, float Pitch);

	// 移動
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	// ジャンプ開始
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpStart();

	// ジャンプ終了
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpEnd();

public:

	// 一人称メッシュを取得する
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	// 一人称カメラを取得する
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};
