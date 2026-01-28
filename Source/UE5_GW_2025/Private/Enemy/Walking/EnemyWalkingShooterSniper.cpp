#include "Enemy/Walking/EnemyWalkingShooterSniper.h"
#include "Enemy/EnemyAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Enemy/EnemyStateMachineComponent.h"

// もしLineDrawerを使うならインクルード
#include "Enemy/MyLineDrawer.h"

AEnemyWalkingShooterSniper::AEnemyWalkingShooterSniper()
{
	// 親クラスの設定
	PrimaryActorTick.bCanEverTick = true;

	// --- 1. Laser Mesh Component の作成 ---
	LaserMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserMesh"));
	LaserMeshComponent->SetupAttachment(RootComponent);

	// コリジョンと影を無効化（ゲームプレイに影響させないため）
	LaserMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserMeshComponent->SetCastShadow(false);

	// ★重要: ここにあった ConstructorHelpers::FObjectFinder は全て削除しました。
	// これにより、パッケージ化時にパスが見つからないエラーや、アセットのロード失敗を防ぎます。
	// メッシュとマテリアルの設定は BeginPlay で行います。

	// デフォルトは非表示
	LaserMeshComponent->SetVisibility(false);
}

void AEnemyWalkingShooterSniper::BeginPlay()
{
	Super::BeginPlay();

	// --- アセットの適用 (Shipping対策) ---
	if (LaserMeshComponent)
	{
		// ブループリントで設定されたメッシュ (Cylinder) を適用
		if (LaserBaseMesh)
		{
			LaserMeshComponent->SetStaticMesh(LaserBaseMesh);
		}
		else
		{
			// 設定忘れ防止のログ
			UE_LOG(LogTemp, Warning, TEXT("Sniper: LaserBaseMesh is not set in Blueprint! Laser will not show."));
		}

		// ブループリントで設定されたマテリアル (M_SniperLaser) を適用
		if (LaserBaseMaterial)
		{
			LaserMeshComponent->SetMaterial(0, LaserBaseMaterial);

			// 色を変えるためにダイナミックインスタンスを作成
			LaserMaterialInstance = LaserMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
		}
	}

	// --- レーザーの発光設定 ---
	if (LaserMaterialInstance)
	{
		// パラメータ名 "Color" を赤色（高輝度）に設定
		// RGB値を 1.0 以上にすることで Unlit マテリアルを発光(Bloom)させる
		LaserMaterialInstance->SetVectorParameterValue(FName("Color"), FLinearColor(50.0f, 0.0f, 0.0f, 1.0f));
	}

	// --- スナイパーパラメータの設定 ---
	FireRange = SniperFireRange;
	BurstCount = SniperBurstCount;
	FireCycleInterval = SniperFireCycleInterval - BurstInterval;

	// --- AI視界設定 ---
	if (Controller)
	{
		AEnemyAIController* EnemyAI = Cast<AEnemyAIController>(Controller);
		if (EnemyAI)
		{
			EnemyAI->SetSightRadius(SniperFireRange);
		}
	}

	// --- デバッグ用LineDrawer (必要なら) ---
	// Shippingでは通常Spawnされないように条件分岐するか、そのままでも可
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

	// 親クラスの BulletClass が設定されているか確認
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
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Sniper: BulletClass is NONE! Check Blueprint settings."));
	}
}

void AEnemyWalkingShooterSniper::AllowLaserDisplay()
{
	bCanShowLaser = true;
}

void AEnemyWalkingShooterSniper::ShowAimPredictor(float DeltaTime)
{
	AActor* Target = StateMachine->GetTarget();

	// ターゲットがいない、または発射直後のクールダウン中、またはコンポーネントが無効なら非表示
	if (!Target || !bCanShowLaser || !LaserMeshComponent || !LaserMeshComponent->GetStaticMesh())
	{
		if (LaserMeshComponent) LaserMeshComponent->SetVisibility(false);
		return;
	}

	// --- A. 座標計算 ---
	FVector StartLocation = MuzzleLocation; // 銃口の位置 (Tick毎に更新されている前提)

	// ターゲットの位置
	FVector TargetLocation = Target->GetActorLocation();

	// 銃口からターゲットへのベクトル
	FVector BeamVector = TargetLocation - StartLocation;
	float Distance = BeamVector.Size();

	// 射程距離より遠い場合の制限
	if (Distance > FireRange)
	{
		BeamVector = BeamVector.GetSafeNormal() * FireRange;
		Distance = FireRange;
		TargetLocation = StartLocation + BeamVector;
	}

	// --- B. メッシュの変形 ---

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