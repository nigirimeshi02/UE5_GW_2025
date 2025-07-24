// EnemyStateMachineComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyState.h"
#include "EnemyStateMachineComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE5_GW_2025_API UEnemyStateMachineComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEnemyStateMachineComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** ���݂̏�� */
    UPROPERTY(BlueprintReadOnly)
    EEnemyState CurrentState;

    /** ��Ԃ�ύX����֐� */
    UFUNCTION(BlueprintCallable)
    void ChangeState(EEnemyState NewState);

    /** �v���C���[�^�[�Q�b�g */
    UPROPERTY(BlueprintReadOnly)
    AActor* TargetActor;

    void SetTarget(AActor* NewTarget);

private:
    void HandleStateIdle(float DeltaTime);
    void HandleStatePatrol(float DeltaTime);
    void HandleStateChase(float DeltaTime);
    void HandleStateAttack(float DeltaTime);
    void HandleStateDead(float DeltaTime);
};
