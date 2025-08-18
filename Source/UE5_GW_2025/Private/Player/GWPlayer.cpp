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
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

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

	// create the noise emitter component
	PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Pawn Noise Emitter"));

	// キャラクターコンポーネントの設定をする
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// キャラクターの移動の設定をする
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);

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

	IsReload = false;
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

	TArray<FAbilityInfo> AbilityList = {
		{ TEXT("/Game/GameplayAbilitySystem/Abilities/GA_GrapplingHook"),   1, 0 },
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
	// ignore if already dead
	if (CurrentHP <= 0.0f)
	{
		return 0.0f;
	}

	// Reduce HP
	CurrentHP -= Damage;

	// Have we depleted HP?
	if (CurrentHP <= 0.0f)
	{
		// deactivate the weapon
		if (IsValid(CurrentWeapon))
		{
			CurrentWeapon->DeactivateWeapon();
		}


		// reset the bullet counter UI
		OnMagazineUpdated.Broadcast(0, 0);

		// destroy this character
		Destroy();
	}

	return Damage;
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
	// stop firing the current weapon
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

	// ensure we have at least two weapons two switch between
	if (OwnedWeapons.Num() > 1)
	{
		// deactivate the old weapon
		CurrentWeapon->DeactivateWeapon();

		// find the index of the current weapon in the owned list
		int32 WeaponIndex = OwnedWeapons.Find(CurrentWeapon);

		// is this the last weapon?
		if (WeaponIndex == OwnedWeapons.Num() - 1)
		{
			// loop back to the beginning of the array
			WeaponIndex = 0;
		}
		else {
			// select the next weapon index
			++WeaponIndex;
		}

		// set the new weapon as current
		CurrentWeapon = OwnedWeapons[WeaponIndex];

		// activate the new weapon
		CurrentWeapon->ActivateWeapon();
	}
}

void AGWPlayer::DoReloadStart()
{
	// 装備中かつリロード中ではないなら
	if (CurrentWeapon && !IsReload)
	{
		IsReload = true;
		CurrentWeapon->Reload();
	}
}

void AGWPlayer::DoReloadEnd(UAnimMontage* Montage, bool bInterrupted)
{
	IsReload = false;
}

void AGWPlayer::AttachWeaponMeshes(AShootingWeapon* Weapon)
{
	const FAttachmentTransformRules AttachmentRule(EAttachmentRule::SnapToTarget, false);

	// attach the weapon actor
	Weapon->AttachToActor(this, AttachmentRule);

	// attach the weapon meshes
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
	// apply the recoil as pitch input
	AddControllerPitchInput(Recoil);
}

void AGWPlayer::UpdateWeaponHUD(int32 CurrentAmmo, int32 MagazineSize)
{
	OnMagazineUpdated.Broadcast(MagazineSize, CurrentAmmo);
}

FVector AGWPlayer::GetWeaponTargetLocation()
{
	// trace ahead from the camera viewpoint
	FHitResult OutHit;

	const FVector Start = GetFirstPersonCameraComponent()->GetComponentLocation();
	const FVector End = Start + (GetFirstPersonCameraComponent()->GetForwardVector() * MaxAimDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, QueryParams);

	// return either the impact point or the trace end
	return OutHit.bBlockingHit ? OutHit.ImpactPoint : OutHit.TraceEnd;
}

void AGWPlayer::AddWeaponClass(const TSubclassOf<AShootingWeapon>& WeaponClass)
{
	// do we already own this weapon?
	AShootingWeapon* OwnedWeapon = FindWeaponOfType(WeaponClass);

	if (!OwnedWeapon)
	{
		// spawn the new weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;

		AShootingWeapon* AddedWeapon = GetWorld()->SpawnActor<AShootingWeapon>(WeaponClass, GetActorTransform(), SpawnParams);

		if (AddedWeapon)
		{
			// add the weapon to the owned list
			OwnedWeapons.Add(AddedWeapon);

			// if we have an existing weapon, deactivate it
			if (CurrentWeapon)
			{
				CurrentWeapon->DeactivateWeapon();
			}

			// switch to the new weapon
			CurrentWeapon = AddedWeapon;
			CurrentWeapon->ActivateWeapon();
		}
	}
}

void AGWPlayer::OnWeaponActivated(AShootingWeapon* Weapon)
{
	// update the bullet counter
	OnMagazineUpdated.Broadcast(Weapon->GetMagazineSize(), Weapon->GetBulletCount());

	// set the character mesh AnimInstances
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

AShootingWeapon* AGWPlayer::FindWeaponOfType(TSubclassOf<AShootingWeapon> WeaponClass) const
{
	// check each owned weapon
	for (AShootingWeapon* Weapon : OwnedWeapons)
	{
		if (Weapon->IsA(WeaponClass))
		{
			return Weapon;
		}
	}

	// weapon not found
	return nullptr;

}