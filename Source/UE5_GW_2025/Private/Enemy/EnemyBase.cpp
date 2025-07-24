// EnemyBase.cpp
#include "Enemy/EnemyBase.h"
#include "Enemy/EnemyStateMachineComponent.h"
#include "Enemy/EnemyWeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;

    StateMachine = CreateDefaultSubobject<UEnemyStateMachineComponent>(TEXT("StateMachine"));
    WeaponComponent = CreateDefaultSubobject<UEnemyWeaponComponent>(TEXT("WeaponComponent"));
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AEnemyBase::OnPlayerSpotted(APawn* PlayerPawn)
{
    StateMachine->SetTarget(PlayerPawn);
    StateMachine->ChangeState(EEnemyState::Chase);
}

void AEnemyBase::AttackTarget()
{
    WeaponComponent->PerformAttack();
}

void AEnemyBase::MoveToTarget(APawn* Target)
{
    // AIController 経由で MoveTo を行う想定
}
