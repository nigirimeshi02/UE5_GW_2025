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
        MyPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
    }
}

void AEnemyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // �R���g���[�����Ă���Pawn���擾
    APawn* ControlledPawn = GetPawn();

    if (TargetPawn && ControlledPawn)
    {
        // ��s�^�C�v�Ȃ� MoveToActor ���g��Ȃ�
        if (ControlledPawn->IsA(AEnemyFlying::StaticClass()))
        {
            // ��s�L�����͎��O�œ��� �� AIController�͈ړ��w�����Ȃ�
            //return;
            //UE_LOG(LogTemp, Log, TEXT("Move Actor!"));
        }
        else 
        {
            // �v���C���[�����E�ɓ����Ă���Ԉړ�
            MoveToActor(TargetPawn, AcceptanceRadius);
        }
       
        //UE_LOG(LogTemp, Log, TEXT("Move Actor!"));
    }

    // Tick �ɂ����ǉ����ėl�q������
    //UE_LOG(LogTemp, Log, TEXT("Enemy: %s, Target: %s"), *GetName(), TargetPawn ? *TargetPawn->GetName() : TEXT("None"));

    if (GetPawn())
    {
		// ���o���m�̃f�o�b�O�\��
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
        if (Actor->ActorHasTag(TEXT("Player"))) { // ����Enemy�^�O��ݒ�

            AEnemyBase* Enemy = Cast<AEnemyBase>(GetPawn());
            if (!Enemy) return;

            if (Stimulus.WasSuccessfullySensed())
            {
                // �v���C���[�����m���ꂽ�i���E�ɓ������j
                UE_LOG(LogTemp, Warning, TEXT("Player spotted again"));
                Enemy->OnPlayerSpotted(PlayerPawn);
            }
            else
            {
                // �v���C���[�����������i���E����������j
                UE_LOG(LogTemp, Warning, TEXT("Player lost"));
                LastKnownPlayerLocation = Stimulus.StimulusLocation; // �Ō�Ɋ��m�����ʒu��ۑ�
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
		// �ړ������������ꍇ�̏���
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
    MoveToLocation(LastKnownPlayerLocation, 10.0f); // 10cm�ȓ��Œ�~
}

void AEnemyAIController::SetTarget(APawn* NewTarget)
{
    TargetPawn = NewTarget;
}

void AEnemyAIController::SetAcceptanceRadius(float NewAcceptanceRadius)
{
    AcceptanceRadius = NewAcceptanceRadius;
}