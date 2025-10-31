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
#include "Player/GWPlayerState.h"
#include "AbilitySystemComponent.h"
#include "ShooterWeapon.h"
#include "Player/Weapon/ShootingWeapon.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimMontage.h"
#include "Player/GWPlayerController.h"
#include "Abilities/PlayerAttributeSet.h"
#include "Engine/DamageEvents.h"
#include "Game/GWGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

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

	// ノイズエミッターコンポーネントを作成する
	PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Pawn Noise Emitter"));

	// キャラクターコンポーネントの設定をする
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// キャラクターの移動の設定をする
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f);

	// IA_Jumpを読み込む
	JumpAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_Jump"));
	// IA_Moveを読み込む
	MoveAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_Move"));
	// IA_MouseLookを読み込む
	LookAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_MouseLook"));
	// IA_Shootを読み込む
	FireAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_Shoot"));
	// IA_SwapWeaponを読み込む
	SwitchWeaponAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_SwapWeapon"));
	// IA_Reloadを読み込む
	ReloadAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_Reload"));

	ClimbDuration = 0.15f;

	LookAtInterpSpeed = 10.0f; // 壁方向へ向く速さ

	IsReload = false;
	IsClimbing = false;
	CanClimb = false;
}

void AGWPlayer::BeginPlay()
{
	Super::BeginPlay();

	AGWPlayerController* GWPC = Cast<AGWPlayerController>(GetController());

	if (GWPC)
	{
		AGWPlayerState* GWPS = Cast<AGWPlayerState>(GWPC->PlayerState);

		if (GWPS)
		{
			UPlayerAttributeSet* AttributeSet = GWPS->GetAttributeSet();

			UpdateHPHUD(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
		}

	}

	if (InitWeapon)
	{
		AddWeaponClass(InitWeapon);
	}

}

void AGWPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsClimbing)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			// 視線を壁方向に補間
			FRotator CurrentRot = PC->GetControlRotation();
			FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRotation, DeltaTime, LookAtInterpSpeed);
			PC->SetControlRotation(NewRot);
		}
	}
	else
	{
		FVector Start = GetActorLocation();
		FVector Forward = GetActorForwardVector();
		FVector End = Start + Forward * 50.0f; // 前方50cmにRaycast

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this); // 自分自身は無視

		// Trace にて ECC_GameTraceChannel3 (Climbable) を使用
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			ClimbTraceHit, Start, End, ECC_GameTraceChannel3, Params
		);

		// デバッグ表示（赤い線でトレース確認）
		//DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 0.1f, 0, 1.0f);

		CanClimb = bHit;
	}

	// 現在のリコイルを滑らかに目標値へ補間
	CurrentRecoilPitch = FMath::FInterpTo(CurrentRecoilPitch, TargetRecoilPitch, DeltaTime, RecoilRecoverySpeed);

	// 視点を補正
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		float DeltaPitch = TargetRecoilPitch - CurrentRecoilPitch;
		PC->AddPitchInput(-DeltaPitch); // 戻る方向に回転
	}

	// 目標値を0に戻していく
	TargetRecoilPitch = FMath::FInterpTo(TargetRecoilPitch, 0.0f, DeltaTime, RecoilRecoverySpeed * 0.5f);
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

		// 壁のぼり
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Ongoing, this, &AGWPlayer::TryStartClimb);

		// 移動
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGWPlayer::MoveInput);

		// 視点操作
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGWPlayer::LookInput);

		// Firing
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AGWPlayer::DoStartFiring);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AGWPlayer::DoStopFiring);

		// Switch weapon
		EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Triggered, this, &AGWPlayer::DoSwitchWeapon);

		// Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AGWPlayer::DoReloadStart);
	}
}

void AGWPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// プレイヤーステートを取得し、そこから ASC を取得
	AGWPlayerState* PS = GetPlayerState<AGWPlayerState>();
	if (PS)
	{
		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
		if (ASC)
		{
			// GAS の必要情報（OwnerActor, AvatarActor）を設定
			ASC->InitAbilityActorInfo(PS, this);

			// サーバー側でアビリティの登録を行う（クライアントでは不要）
			if (HasAuthority())
			{
				InitializeAbilities();
			}
		}
	}
}

void AGWPlayer::InitializeAbilities()
{
	AGWPlayerState* PS = GetPlayerState<AGWPlayerState>();
	if (!PS) return;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;

	// Ability 情報を配列で定義（クラス、レベル、スロットID）
	struct FAbilityInfo
	{
		FString Path;     // アセットパス
		int32 Level;      // アビリティのレベル
		int32 InputID;    // 入力ID（0～）
	};

	TArray<FAbilityInfo> AbilityList = 
	{
		 //{ TEXT("/Game/Abilities/GA_Dash"),   1, 0 },
	};

	// それぞれのアビリティを登録
	for (const FAbilityInfo& Info : AbilityList)
	{
		ConstructorHelpers::FClassFinder<UGameplayAbility> AbilityBP(*Info.Path);
		if (AbilityBP.Succeeded())
		{
			// AbilitySpec を作って登録（InputID はキー操作用）
			FGameplayAbilitySpec Spec(AbilityBP.Class, Info.Level, Info.InputID);
			ASC->GiveAbility(Spec);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("アビリティ '%s' が見つかりませんでした。"), *Info.Path);
		}
	}
}

float AGWPlayer::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	
	float FinalDamage = Damage;

	AGWPlayerController* GWPC = Cast<AGWPlayerController>(GetController());
	
	AGWPlayerState* GWPS = Cast<AGWPlayerState>(GWPC->PlayerState);

	UPlayerAttributeSet* AttributeSet = GWPS->GetAttributeSet();
	
	// PointDamageEvent か RadialDamageEvent かを判別
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)&DamageEvent;

		// ヒットしたボーン名を取得
		FName HitBone = PointDamageEvent->HitInfo.BoneName;

		UE_LOG(LogTemp, Log, TEXT("Hit Bone: %s"), *HitBone.ToString());

		// ヘッドショット判定（ボーン名が "head" の場合）
		if (HitBone == FName("head"))
		{
			FinalDamage *= 2.0f; // 2倍ダメージ
		}
	}

	AttributeSet->SetHealth(AttributeSet->GetHealth() - FinalDamage);

	UpdateHPHUD(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());

	// HPがなくなった？
	if (AttributeSet->GetHealth() <= 0.0f)
	{
		// 武器を無効にする
		if (IsValid(CurrentWeapon))
		{
			CurrentWeapon->DeactivateWeapon();
		}

		// 弾数UIを更新
		UpdateWeaponHUD(0, 0, CurrentWeapon->GetInfiniteAmmo());

		UpdateHPHUD(0, 0);

		AGWGameMode* GWGM = Cast<AGWGameMode>(UGameplayStatics::GetGameMode(this));

		if (GWGM)
		{
			// 残機を減らす
			GWGM->DecreaseLife();

			// 残機があるなら処理をする
			if (GWGM->PlayerLife > 0)
			{
				AttributeSet->Initialize();

				UpdateHPHUD(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());

				// このキャラクターを破壊する
				Destroy();
			}

		}

		return 0.f;
	}

	return FinalDamage;
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
	if (GetController() && !IsClimbing)
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

void AGWPlayer::DoStartFiring()
{
	// 現在の装備中の武器で射撃
	if (CurrentWeapon && !IsReload)
	{		
		CurrentWeapon->StartFiring();
	}
}

void AGWPlayer::DoStopFiring()
{
	// 現在の武器の発射を停止する
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
	}
}

void AGWPlayer::DoSwitchWeapon()
{
	if (IsReload)
	{
		return;
	}

	// 少なくとも2つの武器を持っているなら切り替える
	if (OwnedWeapons.Num() > 1)
	{
		// 古い武器を無効にする
		CurrentWeapon->DeactivateWeapon();

		// 所有リスト内の現在の武器のインデックスを見つける
		int32 WeaponIndex = OwnedWeapons.Find(CurrentWeapon);

		// これが最後の武器ですか？
		if (WeaponIndex == OwnedWeapons.Num() - 1)
		{
			// 配列の先頭に戻る
			WeaponIndex = 0;
		}
		else {
			// 次の武器インデックスを選択
			++WeaponIndex;
		}

		// 新しい武器を現在の武器として設定する
		CurrentWeapon = OwnedWeapons[WeaponIndex];

		// 新しい武器を起動する
		CurrentWeapon->ActivateWeapon();
	}
}

void AGWPlayer::DoReloadStart()
{
	Reload();
}

void AGWPlayer::DoReloadEnd(UAnimMontage* Montage, bool bInterrupted)
{
	IsReload = false;

	UpdateWeaponHUD(CurrentWeapon->GetBulletCount(), CurrentWeapon->GetBulletSpare(), CurrentWeapon->GetInfiniteAmmo());
}

void AGWPlayer::TryStartClimb()
{
	if (IsClimbing) return;

	FHitResult Hit;
	if (CheckClimb(Hit))
	{
		DoStartClimb(Hit);
	}
}

void AGWPlayer::DoStartClimb(const FHitResult& Hit)
{
	IsClimbing = true;

	// キャラクター移動を止める
	GetCharacterMovement()->DisableMovement();

	FVector WallNormal = Hit.Normal;
	FVector ClimbStartLocation = GetCapsuleComponent()->GetComponentLocation();

	// 壁の上端を仮定（Apex風：上方向へ100cm＋前方へ15cm）
	FVector UpOffset = FVector::UpVector * 100.0f;
	FVector ForwardOffset = -WallNormal * 15.0f;
	FVector ClimbEndLocation = ClimbStartLocation + UpOffset + ForwardOffset;

	// 壁方向を向かせる
	FRotator NewRot = WallNormal.ToOrientationRotator();
	NewRot.Yaw += 180.f;
	SetActorRotation(NewRot);

	// 壁方向へ向くべき回転を保存
	TargetRotation = WallNormal.ToOrientationRotator();
	TargetRotation.Yaw += 180.0f;

	// モーション再生
	if (ClimbMontage)
	{
		PlayAnimMontage(ClimbMontage);
	}
	// スムーズに上へ移動
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;

	UKismetSystemLibrary::MoveComponentTo(
		GetCapsuleComponent(),
		ClimbEndLocation,
		GetActorRotation(),
		false,  // bEaseOut
		false,  // bEaseIn
		ClimbDuration,
		false, // bForceShortestRotation
		EMoveComponentAction::Move,
		LatentInfo
	);

	// 移動完了後に呼ぶ
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { DoEndClimb(); });
		});
}

void AGWPlayer::DoEndClimb()
{
	if (!IsClimbing)
		return;

	IsClimbing = false;

	// 移動再有効化
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

bool AGWPlayer::CheckClimb(FHitResult& OutHit)
{
	FVector Start = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	FVector End = Start + Forward * 50.0f; // 前方50cmにRaycast

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // 自分自身は無視

	// Trace にて ECC_GameTraceChannel3 (Climbable) を使用
	bool Hit = GetWorld()->LineTraceSingleByChannel(
		OutHit, Start, End, ECC_GameTraceChannel3, Params
	);

	// デバッグ表示（赤い線でトレース確認）
	//DrawDebugLine(GetWorld(), Start, End, Hit ? FColor::Green : FColor::Red, false, 0.1f, 0, 1.0f);

	return Hit;
}

void AGWPlayer::AttachWeaponMeshes(AShootingWeapon* Weapon)
{
	const FAttachmentTransformRules AttachmentRule(EAttachmentRule::SnapToTarget, false);

	// 武器をアクターにアタッチする
	Weapon->AttachToActor(this, AttachmentRule);

	// 武器をメッシュにアタッチする
	Weapon->GetFirstPersonMesh()->AttachToComponent(GetFirstPersonMesh(), AttachmentRule, FirstPersonWeaponSocket);
	Weapon->GetThirdPersonMesh()->AttachToComponent(GetMesh(), AttachmentRule, ThirdPersonWeaponSocket);
}

void AGWPlayer::PlayFiringMontage(UAnimMontage* Montage)
{
	PlayAnimMontage(Montage);
}

void AGWPlayer::PlayReloadMontage(UAnimMontage* Montage)
{
	PlayAnimMontage(Montage);

	if (UAnimMontage* MontageToPlay = Montage)
	{
		// 一人称メッシュからアニメーションインスタンスを取得
		UAnimInstance* AnimInstance = GetFirstPersonMesh()->GetAnimInstance();

		// アニメーションインスタンスが有効かつ、対象モンタージュが再生中でない場合
		if (AnimInstance && !AnimInstance->Montage_IsPlaying(MontageToPlay))
		{
			// モンタージュを再生（再生速度 = 1.0f）
			float Duration = AnimInstance->Montage_Play(MontageToPlay, 1.0f);

			// 終了時に呼び出すデリゲートを作成・バインド
			FOnMontageEnded MontageEndedDelegate;
			MontageEndedDelegate.BindUObject(this, &AGWPlayer::DoReloadEnd);

			// デリゲートを設定（指定モンタージュに対して）
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
		}
	}
}

void AGWPlayer::AddWeaponRecoil(float Recoil)
{
	// 銃を撃った瞬間の上方向のリコイル追加
	TargetRecoilPitch += Recoil * RecoilKickStrength;
}

void AGWPlayer::UpdateWeaponHUD(int32 CurrentAmmo, int32 MagazineSize, bool Infinite)
{
	OnMagazineUpdated.Broadcast(MagazineSize, CurrentAmmo, Infinite);
}

FVector AGWPlayer::GetWeaponTargetLocation()
{
	// カメラの視点から前方をトレースする
	FHitResult OutHit;

	const FVector Start = GetFirstPersonCameraComponent()->GetComponentLocation();
	const FVector End = Start + (GetFirstPersonCameraComponent()->GetForwardVector() * MaxAimDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, QueryParams);

	// インパクトポイントまたはトレースの終了点のいずれかを返す
	return OutHit.bBlockingHit ? OutHit.ImpactPoint : OutHit.TraceEnd;
}

void AGWPlayer::AddWeaponClass(const TSubclassOf<AShootingWeapon>& WeaponClass)
{
	// すでにこの武器を所有しているか確認
	AShootingWeapon* OwnedWeapon = FindWeaponOfType(WeaponClass);

	if (!OwnedWeapon)
	{
		// 新しい武器を生成
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;

		AShootingWeapon* AddedWeapon = GetWorld()->SpawnActor<AShootingWeapon>(WeaponClass, GetActorTransform(), SpawnParams);

		if (AddedWeapon)
		{
			// 武器を所持する
			OwnedWeapons.Add(AddedWeapon);

			// 既存の武器がある場合無効にする
			if (CurrentWeapon)
			{
				CurrentWeapon->DeactivateWeapon();
			}

			// 新しい武器に切り替える
			CurrentWeapon = AddedWeapon;
			CurrentWeapon->ActivateWeapon();
		}
	}
}

void AGWPlayer::OnWeaponActivated(AShootingWeapon* Weapon)
{
	// 弾数UIを更新
	UpdateWeaponHUD(Weapon->GetBulletCount(), Weapon->GetBulletSpare(), Weapon->GetInfiniteAmmo());

	// AnimInstancesをセット
	GetFirstPersonMesh()->SetAnimInstanceClass(Weapon->GetFirstPersonAnimInstanceClass());
	GetMesh()->SetAnimInstanceClass(Weapon->GetThirdPersonAnimInstanceClass());
}

void AGWPlayer::OnWeaponDeactivated(AShootingWeapon* Weapon)
{
	// unused
}

void AGWPlayer::OnSemiWeaponRefire()
{
	// unused
}

void AGWPlayer::Reload()
{
	// 予備の弾がないならリロードしない
	if (CurrentWeapon->GetBulletSpare() <= 0)
	{
		return;
	}

	// 装備中かつリロード中ではないなら
	if (CurrentWeapon && !IsReload)
	{
		IsReload = true;
		CurrentWeapon->Reload();
	}
}

AShootingWeapon* AGWPlayer::FindWeaponOfType(TSubclassOf<AShootingWeapon> WeaponClass) const
{
	// 所有している武器を確認
	for (AShootingWeapon* Weapon : OwnedWeapons)
	{
		if (Weapon->IsA(WeaponClass))
		{
			return Weapon;
		}
	}

	// 武器が見つからない
	return nullptr;

}

void AGWPlayer::UpdateHPHUD(int32 CurrentHP, int32 MaxHP)
{
	OnHPBarUpdated.Broadcast(MaxHP, CurrentHP);
}
