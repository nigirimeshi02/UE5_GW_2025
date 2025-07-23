// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GWPlayer.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"

AGWPlayer::AGWPlayer()
{
 	// 毎フレーム呼ぶ
	PrimaryActorTick.bCanEverTick = true;

	// カプセルコリジョンのサイズを設定する
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// 一人称メッシュを作成する
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// カメラを作成する
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// キャラクターコンポーネントの設定をする
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// キャラクターの移動の設定をする
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	// IA_Jumpを読み込む
	JumpAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_Jump"));
	// IA_Moveを読み込む
	MoveAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_Move"));
	/// IA_MouseLookを読み込む
	LookAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_MouseLook"));
}

void AGWPlayer::BeginPlay()
{
	Super::BeginPlay();

}

void AGWPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGWPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// アクションのバインドを設定する
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// ジャンプ
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGWPlayer::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGWPlayer::DoJumpEnd);

		// 移動
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGWPlayer::MoveInput);

		// 視点操作
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGWPlayer::LookInput);
	}
}

void AGWPlayer::MoveInput(const FInputActionValue& Value)
{
	// Vector2Dを取得する
	FVector2D MovementVector = Value.Get<FVector2D>();

	// 値を渡す
	DoMove(MovementVector.X, MovementVector.Y);

}

void AGWPlayer::LookInput(const FInputActionValue& Value)
{
	// Vector2Dを取得する
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// 値を渡す
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AGWPlayer::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// 回転入力を渡す
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AGWPlayer::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// 移動入力を渡す
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AGWPlayer::DoJumpStart()
{
	// キャラクターのJumpを使用
	Jump();
}

void AGWPlayer::DoJumpEnd()
{
	// キャラクターのStopJumpingを使用
	StopJumping();
}

