#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "EnemyAIController.generated.h"

UCLASS()
class UE5_GW_2025_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AEnemyAIController();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

	/** AI�̊��m�R���|�[�l���g */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    TObjectPtr<UAIPerceptionComponent> MyPerceptionComponent;

	// AI�̎��o���m�ݒ�
    UPROPERTY()
    TObjectPtr<UAISenseConfig_Sight> SightConfig;

	// ���m�X�V���̃C�x���g�n���h��
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// �Ō�Ɋ��m�����v���C���[�̈ʒu
    UPROPERTY()
    FVector LastKnownPlayerLocation;

    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
public:
    // �^�[�Q�b�g�̍ŏI�ʒu�Ɉړ�
    UFUNCTION()
    void MoveToLastKnownLocation();

    /** �^�[�Q�b�g�ݒ� */
    UFUNCTION(BlueprintCallable)
    void SetTarget(APawn* NewTarget);

    /** �ړ������̂������l */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AcceptanceRadius = 100.0f;

private:
    APawn* TargetPawn;
};
