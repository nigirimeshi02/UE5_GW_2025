#include "Enemy/Walking/EnemyWalkingShooter.h"
#include "Enemy/EnemyStateMachineComponent.h"
#include "Enemy/EnemyWeaponComponent.h""
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AEnemyWalkingShooter::AEnemyWalkingShooter()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyWalkingShooter::BeginPlay()
{
    Super::BeginPlay();

    // ���Ԋu�Ŏˌ������݂�
    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyWalkingShooter::TryShootAtPlayer, FireInterval, true);
}

void AEnemyWalkingShooter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AEnemyWalkingShooter::TryShootAtPlayer()
{
    if (!StateMachine) return;

    if (StateMachine->GetCurrentState() != EEnemyState::Chase &&
        StateMachine->GetCurrentState() != EEnemyState::Attack)
    {
        return; // �U����Ԃ���Ȃ��ƌ����Ȃ�
    }

    AActor* Target = StateMachine->GetTarget();
    if (!Target) return;

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance > FireRange) return;

    // �e�̕����������i�����j
    FRotator LookAt = (Target->GetActorLocation() - GetActorLocation()).Rotation();
    SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));

    // �e�𔭎�
    if (BulletClass)
    {
        FVector MuzzleLocation = GetActorLocation() + GetActorForwardVector() * 100.f + FVector(0, 0, 50.f);
        FActorSpawnParameters SpawnParams;
        GetWorld()->SpawnActor<AActor>(BulletClass, MuzzleLocation, LookAt, SpawnParams);

        UE_LOG(LogTemp, Log, TEXT("EnemyShooter: Fired!"));
    }
}
