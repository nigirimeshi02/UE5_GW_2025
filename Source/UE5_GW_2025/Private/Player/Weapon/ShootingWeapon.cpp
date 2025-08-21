// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Weapon/ShootingWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/Weapon/ShootingProjectile.h"
#include "Player/Weapon/ShootingWeaponHolder.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"

AShootingWeapon::AShootingWeapon()
{
 	// 毎フレーム呼ぶ
	PrimaryActorTick.bCanEverTick = true;

	// ルートコンポーネントの作成
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 一人称視点メッシュの作成
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	FirstPersonMesh->SetupAttachment(RootComponent);

	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	FirstPersonMesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::FirstPerson);
	FirstPersonMesh->bOnlyOwnerSee = true;

	// 三人称視点メッシュの作成
	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Third Person Mesh"));
	ThirdPersonMesh->SetupAttachment(RootComponent);

	ThirdPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	ThirdPersonMesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::WorldSpaceRepresentation);
	ThirdPersonMesh->bOwnerNoSee = true;

	MagazineMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	MagazineMeshComp->SetupAttachment(RootComponent);
}

void AShootingWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// オーナーの破壊時に自身も破壊するようデリゲート登録
	GetOwner()->OnDestroyed.AddDynamic(this, &AShootingWeapon::OnOwnerDestroyed);

	// 武器の所有者をキャストする
	WeaponOwner = Cast<IShootingWeaponHolder>(GetOwner());
	PawnOwner = Cast<APawn>(GetOwner());

	// マガジンを満タンにする
	CurrentBullets = MagazineSize;

	// マガジンをアタッチする
	AttachMagazineMeshes();

	// オーナーに武器メッシュをアタッチさせる
	WeaponOwner->AttachWeaponMeshes(this);

}

void AShootingWeapon::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 発射タイマーを削除してリソース解放
	GetWorld()->GetTimerManager().ClearTimer(RefireTimer);	
}

void AShootingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShootingWeapon::OnOwnerDestroyed(AActor* DestroyedActor)
{
	// 所有者が破壊されたらこの武器も破壊されるようにする
	Destroy();
}

void AShootingWeapon::ActivateWeapon()
{
	// 武器を表示に切り替える
	SetActorHiddenInGame(false);

	// オーナーに通知
	WeaponOwner->OnWeaponActivated(this);
}

void AShootingWeapon::DeactivateWeapon()
{
	// 非アクティブ化中にこの武器を発射しないようにする
	StopFiring();

	// 武器を非表示に切り替える
	SetActorHiddenInGame(true);

	// オーナーに通知
	WeaponOwner->OnWeaponDeactivated(this);
}

void AShootingWeapon::StartFiring()
{
	// 発射フラグをtrueにする
	bIsFiring = true;

	// 前回の射撃からどれくらい時間が経過したかを確認
	// 武器の射撃速度が遅く、プレイヤーがトリガーを連打している場合、再発射速度が不足している可能性がある
	const float TimeSinceLastShot = GetWorld()->GetTimeSeconds() - TimeOfLastShot;

	// RefireRate に応じて連射かどうか制御
	if (TimeSinceLastShot > RefireRate)
	{
		// 発砲
		Fire();

	}
	else
	{
		// フルオートの場合は次の発砲をスケジュールする
		if (bFullAuto)
		{
			GetWorld()->GetTimerManager().SetTimer(RefireTimer, this, &AShootingWeapon::Fire, TimeSinceLastShot, false);
		}

	}
}

void AShootingWeapon::StopFiring()
{
	// 射撃フラグをfalseにする
	bIsFiring = false;

	// 再発射タイマーをクリア
	GetWorld()->GetTimerManager().ClearTimer(RefireTimer);
}

void AShootingWeapon::Reload()
{
	WeaponOwner->PlayReloadMontage(ReloadMontage);

	CurrentBullets = MagazineSize;
}

void AShootingWeapon::Fire()
{
	// 射撃中でなければ中断または残弾数が0以下なら中断
	if (!bIsFiring || CurrentBullets <= 0)
	{
		return;
	}

	// FireProjectile() を呼んで弾を撃つ
	FireProjectile(WeaponOwner->GetWeaponTargetLocation());

	// 前回の射撃時間を更新
	TimeOfLastShot = GetWorld()->GetTimeSeconds();

	// ノイズを発生させてAIに通知
	MakeNoise(ShotLoudness, PawnOwner, PawnOwner->GetActorLocation(), ShotNoiseRange, ShotNoiseTag);

	// フルオート？
	if (bFullAuto)
	{
		// フルオートなら再度 Fire() を予約
		GetWorld()->GetTimerManager().SetTimer(RefireTimer, this, &AShootingWeapon::Fire, RefireRate, false);
	}
	else
	{
		// セミオートなら FireCooldownExpired() を予約
		GetWorld()->GetTimerManager().SetTimer(RefireTimer, this, &AShootingWeapon::FireCooldownExpired, RefireRate, false);
	}
}

void AShootingWeapon::FireCooldownExpired()
{
	// セミオート用のリロード待ち解除通知
	WeaponOwner->OnSemiWeaponRefire();
}

void AShootingWeapon::FireProjectile(const FVector& TargetLocation)
{
	// 弾の発射Transformを CalculateProjectileSpawnTransform() で計算
	FTransform ProjectileTransform = CalculateProjectileSpawnTransform(TargetLocation);

	// ProjectileClass で弾を生成
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = PawnOwner;

	AShootingProjectile* Projectile = GetWorld()->SpawnActor<AShootingProjectile>(ProjectileClass, ProjectileTransform, SpawnParams);

	// 発砲のモンタージュを再生
	WeaponOwner->PlayFiringMontage(FiringMontage);

	// リコイル付加
	WeaponOwner->AddWeaponRecoil(FiringRecoil);

	// 弾薬消費
	--CurrentBullets;

	// 弾が空になったらリロードする
	if (CurrentBullets <= 0)
	{
		//CurrentBullets = MagazineSize;
	}

	// HUD更新
	WeaponOwner->UpdateWeaponHUD(CurrentBullets, MagazineSize);
}

FTransform AShootingWeapon::CalculateProjectileSpawnTransform(const FVector& TargetLocation) const
{
	// マズルの位置を取得
	const FVector MuzzleLoc = FirstPersonMesh->GetSocketLocation(MuzzleSocketName);

	// 目標に向かって少し前方に弾をスポーンさせる
	const FVector SpawnLoc = MuzzleLoc + ((TargetLocation - MuzzleLoc).GetSafeNormal() * MuzzleOffset);

	// ランダムな照準ブレ（AimVariance）を付加してリアリズムを向上
	const FRotator AimRot = UKismetMathLibrary::FindLookAtRotation(SpawnLoc, TargetLocation + (UKismetMathLibrary::RandomUnitVector() * AimVariance));

	// Transformで返す
	return FTransform(AimRot, SpawnLoc, FVector::OneVector);
}

void AShootingWeapon::AttachMagazineMeshes()
{
	if (MagazineMesh.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("MagazineMesh is not set!"));
		return;
	}

	// 同期ロード（即時）
	UStaticMesh* Mesh = MagazineMesh.LoadSynchronous();
	if (!Mesh) return;

	// コンポーネント作成
	MagazineMeshComp->SetStaticMesh(Mesh);
	MagazineMeshComp->RegisterComponent();
	MagazineMeshComp->SetOnlyOwnerSee(true);

	// アタッチのルール
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);

	// 一人称メッシュにアタッチする
	MagazineMeshComp->AttachToComponent(GetFirstPersonMesh(), AttachRules, MagazineAttachSocketName);

	// 三人称メッシュにアタッチする
	//MagazineMeshComp->AttachToComponent(GetThirdPersonMesh(), AttachRules, MagazineAttachSocketName);

	// マガジンのボーンを隠す
	HideMagazineBone();
}

void AShootingWeapon::HideMagazineBone()
{
	// 非表示にする
	GetFirstPersonMesh()->HideBoneByName(MagazineHideBoneName, EPhysBodyOp::PBO_None);
	//GetThirdPersonMesh()->HideBoneByName(MagazineHideBoneName, EPhysBodyOp::PBO_None);
}

const TSubclassOf<UAnimInstance>& AShootingWeapon::GetFirstPersonAnimInstanceClass() const
{
	return FirstPersonAnimInstanceClass;
}

const TSubclassOf<UAnimInstance>& AShootingWeapon::GetThirdPersonAnimInstanceClass() const
{
	return ThirdPersonAnimInstanceClass;
}
