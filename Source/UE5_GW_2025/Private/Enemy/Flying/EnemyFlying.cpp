// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Flying/EnemyFlying.h"

AEnemyFlying::AEnemyFlying()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyFlying::BeginPlay()
{
    Super::BeginPlay();
    
}

//void AEnemyFlying::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    /*if (!StateMachine || StateMachine->GetCurrentState() != EEnemyState::Chase)
//        return;
//
//    AActor* Target = StateMachine->GetTarget();
//    if (!Target) return;
//
//    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
//    FVector Direction = ToTarget.GetSafeNormal();
//
//    FVector NewLocation = GetActorLocation() + Direction * FlySpeed * DeltaTime;
//    SetActorLocation(NewLocation);*/
//
//    if (!StateMachine || StateMachine->GetCurrentState() != EEnemyState::Chase)
//        return;
//
//    AActor* Target = StateMachine->GetTarget();
//    if (!Target) return;
//
//    FVector CurrentLocation = GetActorLocation();
//    FVector TargetLocation = Target->GetActorLocation();
//
//	// �ڕW�ʒu�܂ł̋��������e���a�ȏ�Ȃ�ړ�
//	FVector TargetLength = TargetLocation - CurrentLocation;
//    if (TargetLength.Length() >= AcceptanceRadius) {
//
//        // �v���C���[��XZ����������ǂ�������
//        FVector HorizontalToTarget = FVector(TargetLocation.X, TargetLocation.Y, CurrentLocation.Z) - CurrentLocation;
//        FVector HorizontalDirection = HorizontalToTarget.GetSafeNormal();
//
//        // Z�����̖ڕW�ʒu�̓v���C���[ + HoverHeight
//        float TargetZ = TargetLocation.Z + HoverHeight;
//        float NewZ = FMath::FInterpTo(CurrentLocation.Z, TargetZ, DeltaTime, 2.0f); // �����������������
//
//        // �V�����ʒu���v�Z
//        FVector NewLocation = CurrentLocation + HorizontalDirection * FlySpeed * DeltaTime;
//        NewLocation.Z = NewZ;
//
//        SetActorLocation(NewLocation);
//    }
//}

void AEnemyFlying::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!StateMachine || (StateMachine->GetCurrentState() != EEnemyState::Chase 
                            && StateMachine->GetCurrentState() != EEnemyState::Attack))
        return;

    AActor* Target = StateMachine->GetTarget();
    if (!Target) return;

    FVector CurrentLocation = GetActorLocation();
    FVector TargetLocation = Target->GetActorLocation();
    FVector NewLocation = CurrentLocation; // �ŏI�I�Ȉʒu�������ɑg�ݗ��ĂĂ���

    // --- ��Ƀz�o�����O����iZ��� + �T�C���g�h��j ---
    float TargetZ = TargetLocation.Z + HoverHeight;
    float SmoothedZ = FMath::FInterpTo(CurrentLocation.Z, TargetZ, DeltaTime, 2.0f);

    float Time = GetWorld()->GetTimeSeconds();
    float HoverOffset = FMath::Sin(Time * HoverOscillationSpeed) * HoverAmplitude;

    NewLocation.Z = SmoothedZ + HoverOffset;

    // --- ��苗���ȏ�Ȃ琅�������̈ړ��E��]���s�� ---
    FVector ToTarget = TargetLocation - CurrentLocation;
    if (ToTarget.Length() >= AcceptanceRadius)
    {
        // �����ړ�
        FVector HorizontalToTarget = FVector(TargetLocation.X, TargetLocation.Y, CurrentLocation.Z) - CurrentLocation;
        FVector HorizontalDirection = HorizontalToTarget.GetSafeNormal();

        NewLocation += HorizontalDirection * FlySpeed * DeltaTime;

        // ��]���i�s������
        if (!HorizontalDirection.IsNearlyZero())
        {
            FRotator NewRotation = HorizontalDirection.Rotation();
            SetActorRotation(NewRotation);
        }
    }

    SetActorLocation(NewLocation);
}

