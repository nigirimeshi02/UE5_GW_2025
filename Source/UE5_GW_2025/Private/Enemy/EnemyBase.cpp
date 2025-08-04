// EnemyBase.cpp
#include "Enemy/EnemyBase.h"
#include "Enemy/EnemyStateMachineComponent.h"
#include "Enemy/EnemyWeaponComponent.h"
#include "Enemy/EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
	// �X�e�[�g�}�V���ƕ���R���|�[�l���g�̏�����
    StateMachine = CreateDefaultSubobject<UEnemyStateMachineComponent>(TEXT("StateMachine"));
    WeaponComponent = CreateDefaultSubobject<UEnemyWeaponComponent>(TEXT("WeaponComponent"));
	// AIController�̃N���X��ݒ�
    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	// tags��ݒ�
    Tags.Add(TEXT("Enemy"));
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    // �v���C���[�𔭌������Ƃ��̏���
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
	// �v���C���[�𔭌������Ƃ��̏���
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
	// ��ԑJ�ځF�T����
    StateMachine->ChangeState(EEnemyState::Search);
}


void AEnemyBase::AttackTarget()
{
    WeaponComponent->PerformAttack();
}

void AEnemyBase::MoveToTarget(APawn* Target)
{
    // AIController �o�R�� MoveTo ���s���z��
}

void AEnemyBase::OnSearchComplete()
{
    StateMachine->ChangeState(EEnemyState::Idle);
}

EEnemyState AEnemyBase::GetCurrentState() const
{
    return StateMachine->GetCurrentState(); // ��Ԏ擾
}
