#include "Enemy/Walking/EnemyWalkingShooterSniper.h"
#include "Enemy/EnemyAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Enemy/EnemyStateMachineComponent.h" 


// もしLineDrawerを使うならインクルード
#include "Enemy/MyLineDrawer.h"

AEnemyWalkingShooterSniper::AEnemyWalkingShooterSniper()
{
	// 親クラスの設定
	PrimaryActorTick.bCanEverTick = true;

	// --- 1. Create Laser Mesh Component ---
	LaserMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserMesh"));
	LaserMeshComponent->SetupAttachment(RootComponent);

	// Disable collision and shadows
	LaserMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserMeshComponent->SetCastShadow(false);

	// --- 2. Load Cylinder Mesh ---
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshAsset(TEXT("/Engine/BasicShapes/Cylinder"));
	if (CylinderMeshAsset.Succeeded())
	{
		LaserMeshComponent->SetStaticMesh(CylinderMeshAsset.Object);
	}

	// --- 3. Load Custom Laser Material ---
	// ここにコピーしたパスを貼り付けます (例: /Game/Materials/M_SniperLaser)
	// パス末尾の .M_SniperLaser という拡張子のような部分は削除しても機能しますが、そのままでもOKです
	static ConstructorHelpers::FObjectFinder<UMaterial> LaserMat(TEXT("/Script/Engine.Material'/Game/Enemy/M_SniperLaser.M_SniperLaser'"));

	// もし自作マテリアルが見つからない場合のフォールバック（念のため標準マテリアル）
	static ConstructorHelpers::FObjectFinder<UMaterial> BasicMat(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));

	if (LaserMat.Succeeded())
	{
		LaserMeshComponent->SetMaterial(0, LaserMat.Object);
	}
	else if (BasicMat.Succeeded())
	{
		LaserMeshComponent->SetMaterial(0, BasicMat.Object);
	}

	// Hide by default
	LaserMeshComponent->SetVisibility(false);
}
void AEnemyWalkingShooterSniper::BeginPlay()
{
	Super::BeginPlay();

	// --- スナイパーパラメータの設定 ---
	FireRange = SniperFireRange;
	BurstCount = SniperBurstCount;
	FireCycleInterval = SniperFireCycleInterval - BurstInterval;

	// --- 4. Configure Laser Color/Glow ---
	if (LaserMeshComponent && LaserMeshComponent->GetMaterial(0))
	{
		LaserMaterialInstance = LaserMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
		if (LaserMaterialInstance)
		{
			// エディタで作ったパラメータ名 "Color" を指定
			// RGBの値を大きくするほど光ります (例: 50, 0, 0 はかなり眩しい赤)
			LaserMaterialInstance->SetVectorParameterValue(FName("Color"), FLinearColor(50.0f, 0.0f, 0.0f, 1.0f));
		}
	}

	// コントローラーの視界設定
	if (Controller)
	{
		AEnemyAIController* EnemyAI = Cast<AEnemyAIController>(Controller);
		if (EnemyAI)
		{
			EnemyAI->SetSightRadius(3000.f);
		}
	}

	// デバッグ用LineDrawer (必要なら残す)
	if (GetWorld())
	{
		LineDrawer = GetWorld()->SpawnActor<AMyLineDrawer>(AMyLineDrawer::StaticClass());
	}
}

void AEnemyWalkingShooterSniper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 攻撃状態のときのみレーザー更新処理を行う
	if (StateMachine && StateMachine->GetCurrentState() == EEnemyState::Attack)
	{
		ShowAimPredictor(DeltaTime);
	}
	else
	{
		// 攻撃状態でないなら隠す
		if (LaserMeshComponent) LaserMeshComponent->SetVisibility(false);
	}
}

void AEnemyWalkingShooterSniper::TryShootAtPlayer()
{
	if (!StateMachine || StateMachine->GetCurrentState() != EEnemyState::Attack) return;

	AActor* Target = StateMachine->GetTarget();
	if (!Target || FVector::Dist(GetActorLocation(), Target->GetActorLocation()) > FireRange) return;

	// ターゲットへの方向
	FRotator LookAt = (Target->GetActorLocation() - GetActorLocation()).Rotation();

	// スナイパー特有の微小なブレ
	float YawOffset = FMath::FRandRange(-0.5f, 0.5f);
	float PitchOffset = FMath::FRandRange(-0.3f, 0.3f);
	FRotator SpreadRotation = LookAt + FRotator(PitchOffset, YawOffset, 0.f);

	if (BulletClass)
	{
		FActorSpawnParameters SpawnParams;
		// 弾の発射
		GetWorld()->SpawnActor<AActor>(BulletClass, MuzzleLocation, SpreadRotation, SpawnParams);

		// --- 発射時の処理: レーザーを消す ---
		bCanShowLaser = false;
		if (LaserMeshComponent)
		{
			LaserMeshComponent->SetVisibility(false);
		}

		// クールダウン後に再表示するためのタイマー設定
		GetWorldTimerManager().SetTimer(
			TH_LaserCooldown,
			this,
			&AEnemyWalkingShooterSniper::AllowLaserDisplay,
			LaserHideDuration,
			false
		);
	}
}

void AEnemyWalkingShooterSniper::AllowLaserDisplay()
{
	bCanShowLaser = true;
}

void AEnemyWalkingShooterSniper::ShowAimPredictor(float DeltaTime)
{
	AActor* Target = StateMachine->GetTarget();

	// ターゲットがいない、または発射直後のクールダウン中は非表示
	if (!Target || !bCanShowLaser || !LaserMeshComponent)
	{
		LaserMeshComponent->SetVisibility(false);
		return;
	}

	// --- A. 座標計算 ---
	FVector StartLocation = MuzzleLocation; // 銃口の位置 (Tick毎に更新されている前提)

	// ターゲットの位置
	FVector TargetLocation = Target->GetActorLocation();

	// 銃口からターゲットへのベクトル
	FVector BeamVector = TargetLocation - StartLocation;
	float Distance = BeamVector.Size();

	// 射程距離より遠い場合や、壁に当たった場合などはここで制限可能
	// 今回はシンプルにターゲットまで引く
	if (Distance > FireRange)
	{
		BeamVector = BeamVector.GetSafeNormal() * FireRange;
		Distance = FireRange;
		TargetLocation = StartLocation + BeamVector;
	}

	// --- B. メッシュの変形 (ここが重要) ---

	// メッシュを表示
	LaserMeshComponent->SetVisibility(true);

	// 1. 位置: 円柱の中心を「始点と終点の中間」に置く
	FVector MidPoint = StartLocation + (BeamVector * 0.5f);
	LaserMeshComponent->SetWorldLocation(MidPoint);

	// 2. 回転: 円柱はZ軸(高さ方向)に伸びている形状なので、Z軸をビーム方向に向ける
	FRotator LookAtRot = FRotationMatrix::MakeFromZ(BeamVector).Rotator();
	LaserMeshComponent->SetWorldRotation(LookAtRot);

	// 3. スケール: 
	// Z軸 = 長さ (エンジンのデフォルト円柱は高さ100単位なので、距離/100倍する)
	// X,Y軸 = レーザーの太さ
	float BeamThickness = 0.05f; // 太さ調整 (0.05 = 5%)
	float BeamLengthScale = Distance / 100.0f;

	LaserMeshComponent->SetWorldScale3D(FVector(BeamThickness, BeamThickness, BeamLengthScale));
}