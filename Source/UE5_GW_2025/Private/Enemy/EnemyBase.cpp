// EnemyBase.cpp
#include "Enemy/EnemyBase.h"
#include "Enemy/EnemyStateMachineComponent.h"
#include "Enemy/EnemyWeaponComponent.h"
#include "Enemy/EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

#include "Components/SphereComponent.h"
#include "GameFramework/DamageType.h"
#include "Engine/DamageEvents.h"
#include "Blueprint/UserWidget.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
	// ステートマシンと武器コンポーネントの初期化
    StateMachine = CreateDefaultSubobject<UEnemyStateMachineComponent>(TEXT("StateMachine"));
    WeaponComponent = CreateDefaultSubobject<UEnemyWeaponComponent>(TEXT("WeaponComponent"));
	// AIControllerのクラスを設定
    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	// tagsを設定
    Tags.Add(TEXT("Enemy"));
	// キャラクターの体力の初期化
    PrimaryActorTick.bCanEverTick = true;
    CurrentHealth = MaxHealth; // 初期体力

    // --- 弱点コリジョンを作成 ---
    WeakPointSphere = CreateDefaultSubobject<USphereComponent>(TEXT("WeakPointSphere"));
    WeakPointSphere->SetupAttachment(GetRootComponent());
    WeakPointSphere->InitSphereRadius(WeakPointRadius);
    //WeakPointSphere->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // 頭のあたりに配置
    
    // Mesh（キャラクターのスケルタルメッシュ）にアタッチ
    WeakPointSphere->SetupAttachment(GetMesh());

    WeakPointSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 当たり判定は使わない
    WeakPointSphere->SetHiddenInGame(false); // デバッグ時に可視化したい場合は false
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth; // 初期体力

    // プレイヤーを発見したときの処理
    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        AEnemyAIController* EnemyAI = Cast<AEnemyAIController>(AIController);
        if (EnemyAI)
        {
			EnemyAI->SetAcceptanceRadius(AcceptanceRadius);
            UE_LOG(LogTemp, Log, TEXT("SetAcceptanceRadius"));
        }
    }

    // --- ボーンにアタッチ ---
    if (GetMesh() && WeakPointBoneName != NAME_None)
    {
        WeakPointSphere->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeakPointBoneName);
        UE_LOG(LogTemp, Warning, TEXT("WeakPoint attached to bone: %s"), *WeakPointBoneName.ToString());
    }
}

void AEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    UE_LOG(LogTemp, Log, TEXT("Enemy End"));

    // アクターに関連するタイマーを全てクリア
    // SetTimerで使用した特定のFTimerHandleをクリアするのが理想ですが、
    // ここではアクターが設定したタイマーすべてをクリアできます。
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearAllTimersForObject(this);
    }
}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AEnemyBase::OnPlayerSpotted(APawn* PlayerPawn)
{
	// プレイヤーを発見したときの処理
    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        AEnemyAIController* EnemyAI = Cast<AEnemyAIController>(AIController);
        if (EnemyAI)
        {
            EnemyAI->SetTarget(PlayerPawn);
            UE_LOG(LogTemp, Log, TEXT("Player Spotted!"));
        }
    }

    StateMachine->SetTarget(PlayerPawn);
	// 状態遷移：追跡　死んでなければ
    if (StateMachine->GetCurrentState() != EEnemyState::Dead)
    {
        StateMachine->ChangeState(EEnemyState::Chase);
    }
}

void AEnemyBase::OnPlayerLost()
{
    if (StateMachine)
    {
        StateMachine->ChangeState(EEnemyState::Idle);
        StateMachine->SetTarget(nullptr);
    }

    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        AIController->StopMovement();
        AEnemyAIController* EnemyAI = Cast<AEnemyAIController>(AIController);
        if (EnemyAI)
        {
            EnemyAI->SetTarget(nullptr);
            EnemyAI->MoveToLastKnownLocation();
        }
    }
	// 状態遷移：探し中
    StateMachine->ChangeState(EEnemyState::Search);
}


void AEnemyBase::AttackTarget()
{
	if (StateMachine->GetCurrentState() != EEnemyState::Dead) WeaponComponent->PerformAttack();
}

void AEnemyBase::MoveToTarget(APawn* Target)
{
    // AIController 経由で MoveTo を行う想定
}

void AEnemyBase::OnSearchComplete()
{
    StateMachine->ChangeState(EEnemyState::Idle);
}

EEnemyState AEnemyBase::GetCurrentState() const
{
    return StateMachine->GetCurrentState(); // 状態取得
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentHealth <= 0.0f)
    {
        return 0.0f;
    }

    FVector HitLocation = GetActorLocation();

    // --- DamageEvent からヒット位置を取得 ---
    if (DamageEvent.GetTypeID() == FPointDamageEvent::ClassID)
    {
        const FPointDamageEvent* PointEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
        HitLocation = PointEvent->HitInfo.ImpactPoint;
    }
    else if (DamageEvent.GetTypeID() == FRadialDamageEvent::ClassID)
    {
        const FRadialDamageEvent* RadialEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
        HitLocation = RadialEvent->Origin;
    }

    // --- 弱点のワールド座標を取得 ---
    FVector WeakPointWorld = WeakPointSphere->GetComponentLocation();

    // --- 弱点との距離を計算 ---
    float Distance = FVector::Dist(HitLocation, WeakPointWorld);

    if (Distance <= WeakPointSphere->GetScaledSphereRadius())
    {
        DamageAmount *= WeakPointDamageMultiplier;
        UE_LOG(LogTemp, Warning, TEXT("Weak Point Hit! Damage boosted to %f"), DamageAmount);
    }

    // --- ダメージを適用 ---
    CurrentHealth -= DamageAmount;
    UE_LOG(LogTemp, Warning, TEXT("Enemy took %f damage. CurrentHealth: %f"), DamageAmount, CurrentHealth);

    if (CurrentHealth <= 0.0f)
    {
        Die();
    }

    if (DamageWidgetClass != nullptr)
    {
        // 1. ウィジェットの生成
        UUserWidget* DamageWidget = CreateWidget<UUserWidget>(GetWorld(), DamageWidgetClass);

        if (DamageWidget)
        {
            // 2. ウィジェットをビューポートに追加
            // ワールド空間に表示したい場合は、代わりにUWidgetComponentを使用するか、
            // GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreenなどで工夫が必要です。
            // ここでは簡易的に画面に表示（HUD要素として）します。
            DamageWidget->AddToViewport();

            // ※補足：ダメージ量をウィジェットに渡したい場合
            // DamageWidgetが特定のカスタムクラス（例：UDamageDisplayWidget）の場合、
            // キャストして関数を呼び出すことで値を渡せます。
            // UDamageDisplayWidget* CustomWidget = Cast<UDamageDisplayWidget>(DamageWidget);
            // if (CustomWidget) { CustomWidget->SetDamageText(DamageTaken); }


            // 3. 一定時間後にウィジェットを削除するためのタイマーを設定
            FTimerHandle TimerHandle;
            GetWorldTimerManager().SetTimer(
                TimerHandle,
                [DamageWidget]() // ラムダ関数でウィジェットをキャプチャ
                {
                    if (DamageWidget && DamageWidget->IsInViewport())
                    {
                        DamageWidget->RemoveFromParent();
                    }
                },
                WidgetDisplayDuration, // 設定した表示時間
                false // ループさせない
            );
        }
    }

    return DamageAmount;
}


void AEnemyBase::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("Enemy has died."));

    StateMachine->ChangeState(EEnemyState::Dead);

    // AIの制御を外す
    AController* AIController = GetController();
    if (AIController)
    {
        AIController->UnPossess();
    }

    // 衝突は有効のままにしておく（ラグドールのため）
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // メッシュをラグドール化
    USkeletalMeshComponent* MeshComp = GetMesh();
    if (MeshComp)
    {
        MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));   // Ragdoll用のコリジョンプリセット
        MeshComp->SetSimulatePhysics(true);                   // 物理シミュレーション開始
        MeshComp->SetAllBodiesSimulatePhysics(true);
        MeshComp->WakeAllRigidBodies();
        MeshComp->bBlendPhysics = true;                       // アニメーションから物理へ自然に移行
    }

    // 死亡イベントを通知(EnemyManagerに通知)
    OnEnemyDied.Broadcast();

    // 一定時間後に消滅
    SetLifeSpan(10.0f);
}