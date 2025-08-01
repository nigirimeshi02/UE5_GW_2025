#pragma once

#include "EnemyState.h"
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

    // �X�e�[�g�}�V���R���|�[�l���g
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
    TObjectPtr<UEnemyStateMachineComponent> StateMachine;
	// ����R���|�[�l���g
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
    TObjectPtr<UEnemyWeaponComponent> WeaponComponent;
	// �v���C���[�Ƃǂ��܂ŋ߂Â��邩
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
    float AcceptanceRadius = 100.0f; // �f�t�H���g�l

public:
    virtual void Tick(float DeltaTime) override;

    // �v���C���[�𔭌������Ƃ��Ă΂��
    UFUNCTION(BlueprintCallable)
    virtual void OnPlayerSpotted(APawn* PlayerPawn);

    // �v���C���[�𔭌������Ƃ��Ă΂��
    UFUNCTION(BlueprintCallable)
    virtual void OnPlayerLost();

    UFUNCTION(BlueprintCallable)
    virtual void AttackTarget();

    UFUNCTION(BlueprintCallable)
    virtual void MoveToTarget(APawn* Target);

	// �X�e�[�g�}�V���̏�Ԃ��X�V(idle)
    void OnSearchComplete();

	// ���݂̃X�e�[�g���擾
    EEnemyState GetCurrentState() const;
};
