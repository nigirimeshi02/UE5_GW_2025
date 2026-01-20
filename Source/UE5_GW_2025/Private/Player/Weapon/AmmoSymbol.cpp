// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Weapon/AmmoSymbol.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Player/Weapon/ShootingWeaponHolder.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAmmoSymbol::AAmmoSymbol()
{
	// 毎フレーム Tick() を呼ぶように設定
	PrimaryActorTick.bCanEverTick = true;

	// ルートコンポーネントを作成
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 衝突判定用のスフィアコンポーネントを作成
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionObjectType(ECC_WorldStatic);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 衝突検出イベントに関数を登録
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AAmmoSymbol::OnOverlap);

	// 表示用メッシュを作成
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SphereCollision);
	Mesh->SetCollisionProfileName(FName("NoCollision"));

}

// Called when the game starts or when spawned
void AAmmoSymbol::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAmmoSymbol::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 衝突相手が武器所持者かどうか判定
	if (IShootingWeaponHolder* WeaponHolder = Cast<IShootingWeaponHolder>(OtherActor))
	{
		if (!WeaponHolder->CheckAddAmmo())return;

		WeaponHolder->AddAmmo();

		if (GetSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				GetSound,
				GetActorLocation()
			);
		}

		Destroy();
	}
}

// Called every frame
void AAmmoSymbol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

