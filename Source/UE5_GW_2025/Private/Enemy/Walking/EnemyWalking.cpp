#include "Enemy/Walking/EnemyWalking.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyWalking::AEnemyWalking()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyWalking::BeginPlay()
{
    Super::BeginPlay();

    SetWalkingDefaults();
}

void AEnemyWalking::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ���s�^���L�̃��W�b�N������΂�����
}

void AEnemyWalking::SetWalkingDefaults()
{
    // ���s�X�s�[�h����
    GetCharacterMovement()->MaxWalkSpeed = 200.0f;

    // �n��ړ��̂݋��i��s�������j
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
