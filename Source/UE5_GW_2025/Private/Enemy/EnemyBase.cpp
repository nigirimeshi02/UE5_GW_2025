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
	// �X�e�[�g�}�V���ƕ���R���|�[�l���g�̏�����
    StateMachine = CreateDefaultSubobject<UEnemyStateMachineComponent>(TEXT("StateMachine"));
    WeaponComponent = CreateDefaultSubobject<UEnemyWeaponComponent>(TEXT("WeaponComponent"));
	// AIController�̃N���X��ݒ�
    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	// tags��ݒ�
    Tags.Add(TEXT("Enemy"));
	// �L�����N�^�[�̗̑͂̏�����
    PrimaryActorTick.bCanEverTick = true;
    CurrentHealth = MaxHealth; // �����̗�
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

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentHealth <= 0.0f)
    {
        return 0.0f; // ���łɎ��S���Ă���ꍇ�͖���
    }

    // �_���[�W�K�p
    CurrentHealth -= DamageAmount;
    UE_LOG(LogTemp, Warning, TEXT("Enemy took %f damage. CurrentHealth: %f"), DamageAmount, CurrentHealth);

    // ���S�`�F�b�N
    if (CurrentHealth <= 0.0f)
    {
        Die();
    }

    return DamageAmount;
}

void AEnemyBase::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("Enemy has died."));

    // AI�̐�����O��
    AController* AIController = GetController();
    if (AIController)
    {
        AIController->UnPossess();
    }

    // �Փ˂͗L���̂܂܂ɂ��Ă����i���O�h�[���̂��߁j
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // ���b�V�������O�h�[����
    USkeletalMeshComponent* MeshComp = GetMesh();
    if (MeshComp)
    {
        MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));   // Ragdoll�p�̃R���W�����v���Z�b�g
        MeshComp->SetSimulatePhysics(true);                   // �����V�~�����[�V�����J�n
        MeshComp->SetAllBodiesSimulatePhysics(true);
        MeshComp->WakeAllRigidBodies();
        MeshComp->bBlendPhysics = true;                       // �A�j���[�V�������畨���֎��R�Ɉڍs
    }

    // ��莞�Ԍ�ɏ���
    SetLifeSpan(10.0f);
}