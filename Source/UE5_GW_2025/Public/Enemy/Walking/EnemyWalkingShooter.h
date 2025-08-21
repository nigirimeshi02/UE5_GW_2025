#pragma once

#include "CoreMinimal.h"
#include "Enemy/Walking/EnemyWalking.h"
#include "EnemyWalkingShooter.generated.h"

UCLASS()
class UE5_GW_2025_API AEnemyWalkingShooter : public AEnemyWalking
{
    GENERATED_BODY()

public:
    AEnemyWalkingShooter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // �U������
    void TryShootAtPlayer();

	void StartFireCycle();

	void BurstFire();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FRotator MyRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FVector MuzzleLocation;

protected:
    // �e�̔��ˊԊu�i�b�j
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireInterval = 2.0f;

    // �˒�����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireRange = 1000.0f;

    // �e�ۂ̃N���X
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<AActor> BulletClass;

private:
    FTimerHandle FireTimerHandle;

    FTimerHandle FireCycleTimerHandle;   // �U���T�C�N���i3��������1�b�x�ށj
    FTimerHandle BurstFireTimerHandle;   // �o�[�X�g����1�����Ƃ̃^�C�}�[
    int32 ShotsFired = 0;                // �o�[�X�g���ŉ�����������
    int32 BurstCount = 3;                // 1�o�[�X�g�̔��ː�
    float BurstInterval = 0.2f;          // �A�ˊԊu�i�b�j
    float FireCycleInterval = 1.0f;      // �o�[�X�g���Ƃ̑ҋ@����
};
