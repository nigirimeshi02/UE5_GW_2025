#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AEnemyAIController::AEnemyAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    
    MyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	// ���o���m�̐ݒ�
    SightConfig->SightRadius = 1500.f;
    SightConfig->LoseSightRadius = 2000.f;
    SightConfig->PeripheralVisionAngleDegrees = 90.f;
    SightConfig->SetMaxAge(5.f);

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    MyPerceptionComponent->ConfigureSense(*SightConfig);
    MyPerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

	// �^�[�Q�b�g��������
    if (MyPerceptionComponent)
    {
        MyPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
    }
}

void AEnemyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (TargetPawn)
    {
        MoveToActor(TargetPawn, AcceptanceRadius);
    }

    // Tick �ɂ����ǉ����ėl�q������
    //UE_LOG(LogTemp, Log, TEXT("Enemy: %s, Target: %s"), *GetName(), TargetPawn ? *TargetPawn->GetName() : TEXT("None"));
}

void AEnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    UE_LOG(LogTemp, Log, TEXT("Perception Updated!"));

    for (AActor* Actor : UpdatedActors)
    {
        if (APawn* PlayerPawn = Cast<APawn>(Actor))
        {
            // �R���g���[����Owner�͓G�L����
            if (AEnemyBase* Enemy = Cast<AEnemyBase>(GetPawn()))
            {
                Enemy->OnPlayerSpotted(PlayerPawn);
            }

            break; // 1�l��������\��
        }
    }
}

void AEnemyAIController::SetTarget(APawn* NewTarget)
{
    TargetPawn = NewTarget;
}
