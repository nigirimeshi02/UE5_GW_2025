#include "Enemy/EnemyWeaponComponent.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UEnemyWeaponComponent::UEnemyWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UEnemyWeaponComponent::PerformAttack()
{
    switch (WeaponType)
    {
    case EWeaponType::Melee:
        PerformMeleeAttack();
        break;
    case EWeaponType::Ranged:
        PerformRangedAttack();
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Weapon type not set."));
        break;
    }
}

void UEnemyWeaponComponent::PerformMeleeAttack()
{
    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (OwnerChar && MeleeAttackMontage)
    {
        UAnimInstance* AnimInstance = OwnerChar->GetMesh()->GetAnimInstance();
        if (AnimInstance && !AnimInstance->Montage_IsPlaying(MeleeAttackMontage))
        {
            AnimInstance->Montage_Play(MeleeAttackMontage);
            // 攻撃の当たり判定はモンタージュ中にNotifyなどで行う
        }
    }
}

void UEnemyWeaponComponent::PerformRangedAttack()
{
    if (!ProjectileClass) return;

    AActor* OwnerActor = GetOwner();
    USkeletalMeshComponent* MeshComp = OwnerActor->FindComponentByClass<USkeletalMeshComponent>();
    if (!MeshComp) return;

    FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
    FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwnerActor;

    AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
    if (Projectile)
    {
        UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(Projectile->GetRootComponent());
        if (RootComp)
        {
            FVector LaunchDirection = MuzzleRotation.Vector();
            RootComp->AddImpulse(LaunchDirection * BulletSpeed, NAME_None, true);
        }
    }
}
