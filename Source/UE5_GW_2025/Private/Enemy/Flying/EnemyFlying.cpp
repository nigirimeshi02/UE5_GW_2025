// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Flying/EnemyFlying.h"

AEnemyFlying::AEnemyFlying()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyFlying::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!StateMachine || StateMachine->GetCurrentState() != EEnemyState::Chase)
        return;

    AActor* Target = StateMachine->GetTarget();
    if (!Target) return;

    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
    FVector Direction = ToTarget.GetSafeNormal();

    FVector NewLocation = GetActorLocation() + Direction * FlySpeed * DeltaTime;
    SetActorLocation(NewLocation);
}
