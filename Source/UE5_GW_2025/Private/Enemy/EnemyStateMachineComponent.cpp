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
    // プレイヤーを見つけたらChaseなどに切り替える
}

void UEnemyStateMachineComponent::HandleStatePatrol(float DeltaTime)
{
    // 適当に動く（経路探索やWaypoint使用）
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
            // MoveTo（AIControllerに依頼するなど）
        }
    }
}

void UEnemyStateMachineComponent::HandleStateAttack(float DeltaTime)
{
    // 攻撃アニメーション、射撃処理など（WeaponComponentを使う）
}

void UEnemyStateMachineComponent::HandleStateSearch(float DeltaTime)
{
    // プレイヤーを見失った後の探索処理
}

void UEnemyStateMachineComponent::HandleStateDead(float DeltaTime)
{
    // 何もしない、または死体アニメーション中
}
