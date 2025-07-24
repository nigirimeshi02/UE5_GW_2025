// EnemyBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

class UEnemyStateMachineComponent;
class UEnemyWeaponComponent;

UCLASS()
class UE5_GW_2025_API AEnemyBase : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyBase();

protected:
    virtual void BeginPlay() override;

    // �X�e�[�g�}�V���ƕ��폈��
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
    TObjectPtr<UEnemyStateMachineComponent> StateMachine;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
    TObjectPtr<UEnemyWeaponComponent> WeaponComponent;

    // �v���C���[�𔭌������Ƃ��Ă΂��
    UFUNCTION(BlueprintCallable)
    virtual void OnPlayerSpotted(APawn* PlayerPawn);

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    virtual void AttackTarget();

    UFUNCTION(BlueprintCallable)
    virtual void MoveToTarget(APawn* Target);
};
