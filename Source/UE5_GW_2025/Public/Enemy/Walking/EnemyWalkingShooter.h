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

protected:
    // �e�̔��ˊԊu�i�b�j
    UPROPERTY(EditAnywhere, Category = "Combat")
    float FireInterval = 2.0f;

    // �˒�����
    UPROPERTY(EditAnywhere, Category = "Combat")
    float FireRange = 1000.0f;

    // �e�ۂ̃N���X
    UPROPERTY(EditAnywhere, Category = "Combat")
    TSubclassOf<AActor> BulletClass;

private:
    FTimerHandle FireTimerHandle;
};
