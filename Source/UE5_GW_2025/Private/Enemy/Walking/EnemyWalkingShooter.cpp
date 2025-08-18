#include "Enemy/Walking/EnemyWalkingShooter.h"
#include "Enemy/EnemyStateMachineComponent.h"
#include "Enemy/EnemyWeaponComponent.h"
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

    if (!StateMachine) return;

    AActor* Target = StateMachine->GetTarget();
    if (!Target) return;

    // ��]�e�X�g
    // ���������̃x�N�g��
    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
    ToTarget.Z = 0;

    if (!ToTarget.IsNearlyZero())
    {
        // �^�[�Q�b�g�����̉�]
        FRotator TargetRotation = ToTarget.Rotation();

        // ���݂̉�]�����炩�ɕ��
        FRotator NewRotation = FMath::RInterpTo(
            GetActorRotation(),
            TargetRotation,
            DeltaTime,
            5.0f // �� ��]���x�i�傫���قǑ��������j
        );

        // �e�X�g��]
		MyRotation = NewRotation;

        // �R���g���[���[�̌������X�V�i�A�j��BP���Ŏg�p�j
        if (Controller)
        {
            Controller->SetControlRotation(NewRotation);
			SetActorRotation(NewRotation);
            //UE_LOG(LogTemp, Log, TEXT("%f %f %f"),NewRotation.Roll,NewRotation.Yaw,NewRotation.Pitch);
        }
    }


    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

    // �U���͈͂ɓ������� Attack ��Ԃ�
    if (Distance <= FireRange)
    {
        if (StateMachine->GetCurrentState() != EEnemyState::Attack)
        {
            StateMachine->ChangeState(EEnemyState::Attack);
        }
    }
    else
    {
        // �͈͊O�Ȃ�ǐՂɖ߂�
        if (StateMachine->GetCurrentState() == EEnemyState::Attack)
        {
            StateMachine->ChangeState(EEnemyState::Chase);
        }
    }
}

void AEnemyWalkingShooter::TryShootAtPlayer()
{
    if (!StateMachine) return;

    if (StateMachine->GetCurrentState() != EEnemyState::Attack)
    {
        return; // �U����Ԃ���Ȃ��ƌ����Ȃ�
    }

    AActor* Target = StateMachine->GetTarget();
    if (!Target) return;

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance > FireRange) return;

    // �e�̕����������i�����j
    FRotator LookAt = (Target->GetActorLocation() - GetActorLocation()).Rotation();
    //SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));

    // �e�𔭎�
    if (BulletClass)
    {
        FVector MuzzleLocation = GetActorLocation() + GetActorForwardVector() * 200.f + FVector(0, 0, 50.f);
        FActorSpawnParameters SpawnParams;
        GetWorld()->SpawnActor<AActor>(BulletClass, MuzzleLocation, LookAt, SpawnParams);

        UE_LOG(LogTemp, Log, TEXT("EnemyShooter: Fired!"));
    }
}
