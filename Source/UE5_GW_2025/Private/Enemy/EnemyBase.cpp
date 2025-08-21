// EnemyBase.cpp
#include "Enemy/EnemyBase.h"
#include "Enemy/EnemyStateMachineComponent.h"
#include "Enemy/EnemyWeaponComponent.h"
#include "Enemy/EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

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
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();

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
    StateMachine->ChangeState(EEnemyState::Chase);
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
    WeaponComponent->PerformAttack();
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
        return 0.0f; // すでに死亡している場合は無視
    }

    // ダメージ適用
    CurrentHealth -= DamageAmount;
    UE_LOG(LogTemp, Warning, TEXT("Enemy took %f damage. CurrentHealth: %f"), DamageAmount, CurrentHealth);

    // 死亡チェック
    if (CurrentHealth <= 0.0f)
    {
        Die();
    }

    return DamageAmount;
}

void AEnemyBase::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("Enemy has died."));

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

    // 一定時間後に消滅
    SetLifeSpan(10.0f);
}