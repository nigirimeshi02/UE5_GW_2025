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

    // ˆê’èŠÔŠu‚ÅËŒ‚‚ğ‚İ‚é
    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyWalkingShooter::TryShootAtPlayer, FireInterval, true);
}

void AEnemyWalkingShooter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!StateMachine) return;

    AActor* Target = StateMachine->GetTarget();
    if (!Target) return;

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

    // UŒ‚”ÍˆÍ‚É“ü‚Á‚½‚ç Attack ó‘Ô‚É
    if (Distance <= FireRange)
    {
        if (StateMachine->GetCurrentState() != EEnemyState::Attack)
        {
            StateMachine->ChangeState(EEnemyState::Attack);
        }
    }
    else
    {
        // ”ÍˆÍŠO‚È‚ç’ÇÕ‚É–ß‚·
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
        return; // UŒ‚ó‘Ô‚¶‚á‚È‚¢‚ÆŒ‚‚½‚È‚¢
    }

    AActor* Target = StateMachine->GetTarget();
    if (!Target) return;

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance > FireRange) return;

    // e‚Ì•ûŒü‚ğŒü‚­i‹üj
    FRotator LookAt = (Target->GetActorLocation() - GetActorLocation()).Rotation();
    SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));

    // ’e‚ğ”­Ë
    if (BulletClass)
    {
        FVector MuzzleLocation = GetActorLocation() + GetActorForwardVector() * 100.f + FVector(0, 0, 50.f);
        FActorSpawnParameters SpawnParams;
        GetWorld()->SpawnActor<AActor>(BulletClass, MuzzleLocation, LookAt, SpawnParams);

        UE_LOG(LogTemp, Log, TEXT("EnemyShooter: Fired!"));
    }
}
