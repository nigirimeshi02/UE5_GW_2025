// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Weapon/ShootingWeaponSymbol.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/Weapon/ShootingWeaponHolder.h"
#include "Player/Weapon/ShootingWeapon.h"
#include "Engine/World.h"
#include "TimerManager.h"

AShootingWeaponSymbol::AShootingWeaponSymbol()
{
	// 毎フレーム Tick() を呼ぶように設定
	PrimaryActorTick.bCanEverTick = true;

	// ルートコンポーネントを作成
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 衝突判定用のスフィアコンポーネントを作成
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->SetupAttachment(RootComponent);

	SphereCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 84.0f));
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionObjectType(ECC_WorldStatic);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereCollision->bFillCollisionUnderneathForNavmesh = true;

	// 衝突検出イベントに関数を登録
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AShootingWeaponSymbol::OnOverlap);

	// 表示用メッシュを作成
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SphereCollision);

	Mesh->SetCollisionProfileName(FName("NoCollision"));
}

void AShootingWeaponSymbol::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (FWeaponTable* WeaponData = WeaponType.GetRow<FWeaponTable>(FString()))
	{
		// メッシュを設定
		Mesh->SetStaticMesh(WeaponData->StaticMesh.LoadSynchronous());
	}
}

void AShootingWeaponSymbol::BeginPlay()
{
	Super::BeginPlay();

	if (FWeaponTable* WeaponData = WeaponType.GetRow<FWeaponTable>(FString()))
	{
		// 武器クラスをコピー
		WeaponClass = WeaponData->WeaponToSpawn;
	}
}

void AShootingWeaponSymbol::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// リスポーン用タイマーをクリア
	GetWorld()->GetTimerManager().ClearTimer(RespawnTimer);
}

void AShootingWeaponSymbol::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 衝突相手が武器所持者かどうか判定
	if (IShootingWeaponHolder* WeaponHolder = Cast<IShootingWeaponHolder>(OtherActor))
	{
		// 武器クラスを付与
		WeaponHolder->AddWeaponClass(WeaponClass);

		// メッシュを非表示にする
		SetActorHiddenInGame(true);

		// 衝突判定を無効にする
		SetActorEnableCollision(false);

		// Tick を無効にする
		SetActorTickEnabled(false);

		// 一定時間後にリスポーンするようにタイマーをセット
		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AShootingWeaponSymbol::RespawnPickup, RespawnTime, false);
	}
}

void AShootingWeaponSymbol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShootingWeaponSymbol::RespawnPickup()
{
	// 非表示状態を解除
	SetActorHiddenInGame(false);

	// Blueprint 側の演出処理を呼び出す
	BP_OnRespawn();
}

void AShootingWeaponSymbol::FinishRespawn()
{
	// 衝突判定を有効にする
	SetActorEnableCollision(true);

	// Tick を有効にする
	SetActorTickEnabled(true);
}