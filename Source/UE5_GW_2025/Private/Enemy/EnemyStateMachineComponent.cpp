// EnemyStateMachineComponent.cpp
#include "Enemy/EnemyStateMachineComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

UEnemyStateMachineComponent::UEnemyStateMachineComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CurrentState = EEnemyState::Idle;
    TargetActor = nullptr;
}

void UEnemyStateMachineComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UEnemyStateMachineComponent::ChangeState(EEnemyState NewState)
{
    if (CurrentState != NewState)
    {
        CurrentState = NewState;
        UE_LOG(LogTemp, Log, TEXT("Enemy changed state to: %s"), *UEnum::GetValueAsString(CurrentState));
    }
}

void UEnemyStateMachineComponent::SetTarget(AActor* NewTarget)
{
    TargetActor = NewTarget;
}

AActor* UEnemyStateMachineComponent::GetTarget() const
{
    return TargetActor;
}

EEnemyState UEnemyStateMachineComponent::GetCurrentState() const
{
	return CurrentState;
}

void UEnemyStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    switch (CurrentState)
    {
    case EEnemyState::Idle:
        HandleStateIdle(DeltaTime);
        break;
    case EEnemyState::Patrol:
        HandleStatePatrol(DeltaTime);
        break;
    case EEnemyState::Chase:
        HandleStateChase(DeltaTime);
        break;
    case EEnemyState::Attack:
        HandleStateAttack(DeltaTime);
        break;
	case EEnemyState::Search:
        HandleStateSearch(DeltaTime);
		break;
    case EEnemyState::Dead:
        HandleStateDead(DeltaTime);
        break;
    default:
        break;
    }
}

void UEnemyStateMachineComponent::HandleStateIdle(float DeltaTime)
{
    // �v���C���[����������Chase�Ȃǂɐ؂�ւ���
}

void UEnemyStateMachineComponent::HandleStatePatrol(float DeltaTime)
{
    // �K���ɓ����i�o�H�T����Waypoint�g�p�j
}

void UEnemyStateMachineComponent::HandleStateChase(float DeltaTime)
{
    if (TargetActor)
    {
        AActor* Owner = GetOwner();
        FVector TargetLocation = TargetActor->GetActorLocation();
        FVector OwnerLocation = Owner->GetActorLocation();
        float Distance = FVector::Dist(TargetLocation, OwnerLocation);

        if (Distance < 150.0f)
        {
            ChangeState(EEnemyState::Attack);
        }
        else
        {
            // MoveTo�iAIController�Ɉ˗�����Ȃǁj
        }
    }
}

void UEnemyStateMachineComponent::HandleStateAttack(float DeltaTime)
{
    // �U���A�j���[�V�����A�ˌ������ȂǁiWeaponComponent���g���j
}

void UEnemyStateMachineComponent::HandleStateSearch(float DeltaTime)
{
    // �v���C���[������������̒T������
}

void UEnemyStateMachineComponent::HandleStateDead(float DeltaTime)
{
    // �������Ȃ��A�܂��͎��̃A�j���[�V������
}
