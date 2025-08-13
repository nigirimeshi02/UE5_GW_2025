// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "EnemyFlying.generated.h"

/**
 * 
 */
UCLASS()
class UE5_GW_2025_API AEnemyFlying : public AEnemyBase
{
    GENERATED_BODY()

public:
    AEnemyFlying();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    //// �ǐՑ��x
    //UPROPERTY(EditAnywhere, Category = "Movement")
    //float FlySpeed = 300.0f;

    ///** �z�o�����O���鍂���i�v���C���[����j */
    //UPROPERTY(EditAnywhere, Category = "Flight")
    //float HoverHeight = 300.f;


    // �z�o�����O�̗h��̋����Ƒ���
    UPROPERTY(EditAnywhere, Category = "Flying")
    float HoverAmplitude = 6.0f; // �h�ꕝ�i�㉺�̐U�ꕝ�j

    UPROPERTY(EditAnywhere, Category = "Flying")
    float HoverOscillationSpeed = 2.0f; // �h��̑����i���g���j

    UPROPERTY(EditAnywhere, Category = "Flying")
    float HoverHeight = 200.0f; // �v���C���[����ǂ̍����Ƀz�o�����O���邩

    UPROPERTY(EditAnywhere, Category = "Flying")
    float FlySpeed = 300.0f; // �����ړ��̑��x

    // �v���C���[�����X�g�����Ƃ��Ă΂��
    virtual void OnPlayerLost() override;
};
