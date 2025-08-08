#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyBase.h"
#include "Enemy/Flying/EnemyFlying.h"
#include "Navigation/PathFollowingComponent.h"
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
        MyPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
    }
}

void AEnemyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // コントロールしているPawnを取得
    APawn* ControlledPawn = GetPawn();

    if (TargetPawn && ControlledPawn)
    {
        // 飛行タイプなら MoveToActor を使わない
        if (ControlledPawn->IsA(AEnemyFlying::StaticClass()))
        {
            // 飛行キャラは自前で動く → AIControllerは移動指示しない
            //return;
            //UE_LOG(LogTemp, Log, TEXT("Move Actor!"));
        }
        else 
        {
            // プレイヤーが視界に入っている間移動
            MoveToActor(TargetPawn, AcceptanceRadius);
        }
       
        //UE_LOG(LogTemp, Log, TEXT("Move Actor!"));
    }

    // Tick にこれを追加して様子を見る
    //UE_LOG(LogTemp, Log, TEXT("Enemy: %s, Target: %s"), *GetName(), TargetPawn ? *TargetPawn->GetName() : TEXT("None"));

    if (GetPawn())
    {
		// 視覚感知のデバッグ表示
        FVector Location = GetPawn()->GetActorLocation();
        FRotator Rotation = GetPawn()->GetActorRotation();
        float VisionAngle = SightConfig->PeripheralVisionAngleDegrees;
        float Radius = SightConfig->SightRadius;

        int Segments = 32;
        for (int i = 0; i <= Segments; i++)
        {
            float Angle = -VisionAngle + (2 * VisionAngle) * i / Segments;
            FRotator SegmentRot = FRotator(0, Angle, 0);
            FVector Direction = SegmentRot.RotateVector(GetPawn()->GetActorForwardVector());
            FVector EndPoint = Location + Direction * Radius;

            DrawDebugLine(GetWorld(), Location, EndPoint, FColor::Blue, false, -1.f, 0, 1.f);
        }
    }
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    UE_LOG(LogTemp, Warning, TEXT("Stimulus Success: %s"), Stimulus.WasSuccessfullySensed() ? TEXT("YES") : TEXT("NO"));

    if (APawn* PlayerPawn = Cast<APawn>(Actor))
    {
        if (Actor->ActorHasTag(TEXT("Player"))) { // 仮でEnemyタグを設定

            AEnemyBase* Enemy = Cast<AEnemyBase>(GetPawn());
            if (!Enemy) return;

            if (Stimulus.WasSuccessfullySensed())
            {
                // プレイヤーが感知された（視界に入った）
                UE_LOG(LogTemp, Warning, TEXT("Player spotted again"));
                Enemy->OnPlayerSpotted(PlayerPawn);
            }
            else
            {
                // プレイヤーを見失った（視界から消えた）
                UE_LOG(LogTemp, Warning, TEXT("Player lost"));
                LastKnownPlayerLocation = Stimulus.StimulusLocation; // 最後に感知した位置を保存
                Enemy->OnPlayerLost();
            }
        }
    }
}

void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);
    
    if (Result.Code == EPathFollowingResult::Success)
    {
		// 移動が完了した場合の処理
        AEnemyBase* Enemy = Cast<AEnemyBase>(GetPawn());
        if (Enemy && Enemy->GetCurrentState() == EEnemyState::Search)
        {
            Enemy->OnSearchComplete();
        }
    }
}

void AEnemyAIController::MoveToLastKnownLocation()
{
    UE_LOG(LogTemp, Log, TEXT("MoveToLastKnownLocation: x%f y%f z%f"), LastKnownPlayerLocation.X, LastKnownPlayerLocation.Y, LastKnownPlayerLocation.Z);
    MoveToLocation(LastKnownPlayerLocation, 10.0f); // 10cm以内で停止
}

void AEnemyAIController::SetTarget(APawn* NewTarget)
{
    TargetPawn = NewTarget;
}

void AEnemyAIController::SetAcceptanceRadius(float NewAcceptanceRadius)
{
    AcceptanceRadius = NewAcceptanceRadius;
}