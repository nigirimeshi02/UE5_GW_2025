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

    // ステートマシンと武器処理
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
    TObjectPtr<UEnemyStateMachineComponent> StateMachine;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
    TObjectPtr<UEnemyWeaponComponent> WeaponComponent;

public:
    virtual void Tick(float DeltaTime) override;

    // プレイヤーを発見したとき呼ばれる
    UFUNCTION(BlueprintCallable)
    virtual void OnPlayerSpotted(APawn* PlayerPawn);

    UFUNCTION(BlueprintCallable)
    virtual void AttackTarget();

    UFUNCTION(BlueprintCallable)
    virtual void MoveToTarget(APawn* Target);
};
