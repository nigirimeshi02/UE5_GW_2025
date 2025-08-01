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

    // 歩行型特有のロジックがあればここに
}

void AEnemyWalking::SetWalkingDefaults()
{
    // 歩行スピード調整
    GetCharacterMovement()->MaxWalkSpeed = 200.0f;

    // 地上移動のみ許可（飛行無効化）
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
