#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyWeaponComponent.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    None,
    Melee,
    Ranged
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE5_GW_2025_API UEnemyWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEnemyWeaponComponent();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    EWeaponType WeaponType = EWeaponType::None;

    // çUåÇÇÃé¿çs
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void PerformAttack();

    // ===== ãﬂê⁄çUåÇ =====
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Melee")
    class UAnimMontage* MeleeAttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Melee")
    float MeleeDamage = 20.0f;

    // ===== âìãóó£çUåÇ =====
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Ranged")
    TSubclassOf<AActor> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Ranged")
    FName MuzzleSocketName = "Muzzle";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Ranged")
    float BulletSpeed = 2000.0f;

private:
    void PerformMeleeAttack();
    void PerformRangedAttack();
};
