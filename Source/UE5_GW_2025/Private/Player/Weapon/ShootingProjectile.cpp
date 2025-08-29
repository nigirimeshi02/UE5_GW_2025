// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Weapon/ShootingProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Player/GWPlayerController.h"
#include "Player/GWPlayerState.h"
#include "Abilities/PlayerAttributeSet.h"

AShootingProjectile::AShootingProjectile()
{
	// 毎フレーム Tick 関数を呼び出す
	PrimaryActorTick.bCanEverTick = true;

	// 衝突コンポーネントを作成し、RootComponent に設定
	RootComponent = CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));

	CollisionComponent->SetSphereRadius(16.0f);  // 衝突判定の半径
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);  // 物理および問い合わせ衝突を有効化
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);  // すべてのチャンネルに対してブロック設定
	CollisionComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;  // キャラクターが乗ることを禁止

	// 弾の移動コンポーネントを作成（SceneComponent ではないのでアタッチは不要）
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

	ProjectileMovement->InitialSpeed = 3000.0f;  // 初速
	ProjectileMovement->MaxSpeed = 3000.0f;      // 最大速度
	ProjectileMovement->bShouldBounce = true;    // 反射を有効化

	// ダメージタイプの初期値を設定
	HitDamageType = UDamageType::StaticClass();
}

void AShootingProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 弾を発射したプレイヤー自身は衝突対象から除外
	CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
}

void AShootingProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	// すでに命中している場合は無視
	if (bHit)
	{
		return;
	}

	bHit = true;

	// AI が感知できるノイズを発生させる
	MakeNoise(NoiseLoudness, GetInstigator(), GetActorLocation(), NoiseRange, NoiseTag);

	// 命中対象が物理オブジェクトである場合
	if (OtherComp->IsSimulatingPhysics())
	{
		// 物理的な衝撃を加える
		OtherComp->AddImpulseAtLocation(GetVelocity() * PhysicsForce, Hit.ImpactPoint);
	}

	// 命中対象がキャラクターである場合
	if (ACharacter* HitCharacter = Cast<ACharacter>(Other))
	{
		// 発射元と同じキャラクターでないことを確認
		if (HitCharacter != GetOwner())
		{
			// ダメージを与える
			DamageCharacter(HitCharacter, Hit);
		}
	}

	// 弾の衝突判定を無効化
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Blueprint 側でヒット時のエフェクトなどを処理する
	BP_OnProjectileHit(Hit);
}

void AShootingProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShootingProjectile::DamageCharacter(ACharacter* HitCharacter, const FHitResult& Hit)
{
	AGWPlayerController* GWPC = Cast<AGWPlayerController>(GetOwner()->GetInstigatorController());

	AGWPlayerState* GWPS = Cast<AGWPlayerState>(GWPC->PlayerState);

	float Damage = GWPS->GetAttributeSet()->GetAttackPower() + HitDamage;

	FVector ShotDirection = GetActorForwardVector();

	// GameplayStatics を使ってダメージを適用
	//UGameplayStatics::ApplyDamage(HitCharacter, Damage, GetInstigator()->GetController(), this, HitDamageType);
	UGameplayStatics::ApplyPointDamage(HitCharacter, Damage, ShotDirection, Hit, GWPC, this, HitDamageType);
}
