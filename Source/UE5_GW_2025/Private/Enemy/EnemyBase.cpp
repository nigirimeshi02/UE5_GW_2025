// EnemyBase.cpp
#include "Enemy/EnemyBase.h"
#include "Enemy/EnemyStateMachineComponent.h"
#include "Enemy/EnemyWeaponComponent.h"
#include "Enemy/EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

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
