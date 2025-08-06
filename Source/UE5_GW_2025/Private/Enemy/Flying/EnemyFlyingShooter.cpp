// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Flying/EnemyFlyingShooter.h"

AEnemyFlyingShooter::AEnemyFlyingShooter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyFlyingShooter::BeginPlay()
{
    Super::BeginPlay();
    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyFlyingShooter::TryShootAtPlayer, FireInterval, true);
}

void AEnemyFlyingShooter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AActor* Target = StateMachine->GetTarget();
    if (!Target) return;

    float Dist = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

    if (Dist <= FireRange)
    {
        if (StateMachine->GetCurrentState() != EEnemyState::Attack)
            StateMachine->ChangeState(EEnemyState::Attack);
    }
    else
    {
        if (StateMachine->GetCurrentState() == EEnemyState::Attack)
            StateMachine->ChangeState(EEnemyState::Chase);
    }
}

void AEnemyFlyingShooter::TryShootAtPlayer()
{
    if (!StateMachine || StateMachine->GetCurrentState() != EEnemyState::Attack)
        return;

    AActor* Target = StateMachine->GetTarget();
    if (!Target || !BulletClass) return;

    FVector MuzzleLoc = GetActorLocation() + GetActorForwardVector() * 150.0f;
    FRotator MuzzleRot = (Target->GetActorLocation() - GetActorLocation()).Rotation();

    GetWorld()->SpawnActor<AActor>(BulletClass, MuzzleLoc, MuzzleRot);
}

