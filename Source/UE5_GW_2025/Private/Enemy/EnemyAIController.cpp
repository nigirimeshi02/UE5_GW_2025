#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AEnemyAIController::AEnemyAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    
    MyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	// 視覚感知の設定
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

	// ターゲットを初期化
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

    // Tick にこれを追加して様子を見る
    //UE_LOG(LogTemp, Log, TEXT("Enemy: %s, Target: %s"), *GetName(), TargetPawn ? *TargetPawn->GetName() : TEXT("None"));
}

void AEnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    UE_LOG(LogTemp, Log, TEXT("Perception Updated!"));

    for (AActor* Actor : UpdatedActors)
    {
        if (APawn* PlayerPawn = Cast<APawn>(Actor))
        {
            // コントローラのOwnerは敵キャラ
            if (AEnemyBase* Enemy = Cast<AEnemyBase>(GetPawn()))
            {
                Enemy->OnPlayerSpotted(PlayerPawn);
            }

            break; // 1人見つけたら十分
        }
    }
}

void AEnemyAIController::SetTarget(APawn* NewTarget)
{
    TargetPawn = NewTarget;
}
